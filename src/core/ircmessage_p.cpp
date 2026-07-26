/*
  Copyright (C) 2008-2020 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ircmessage_p.h"
#include <string_view>

IRC_BEGIN_NAMESPACE

#ifndef IRC_DOXYGEN
IrcMessagePrivate::IrcMessagePrivate() :
     timeStamp(QDateTime::currentDateTime())
{
}

QString IrcMessagePrivate::prefix() const
{
    if (!m_prefix.isExplicit() && m_prefix.isNull() && !data.prefix.isNull()) {
        if (data.prefix.startsWith(':')) {
            if (data.prefix.length() > 1)
                m_prefix = QString::fromUtf8(QByteArrayView(data.prefix).sliced(1));
        } else {
            // empty (not null)
            m_prefix = QString("");
        }
    }
    return m_prefix.value();
}

void IrcMessagePrivate::setPrefix(const QString& prefix)
{
    m_prefix.setValue(prefix);
    m_nick.clear();
    m_ident.clear();
    m_host.clear();
}

QString IrcMessagePrivate::nick() const
{
    if (m_nick.isNull())
        parsePrefix(prefix(), &m_nick, &m_ident, &m_host);
    return m_nick;
}

QString IrcMessagePrivate::ident() const
{
    if (m_ident.isNull())
        parsePrefix(prefix(), &m_nick, &m_ident, &m_host);
    return m_ident;
}

QString IrcMessagePrivate::host() const
{
    if (m_host.isNull())
        parsePrefix(prefix(), &m_nick, &m_ident, &m_host);
    return m_host;
}

QString IrcMessagePrivate::command() const
{
    if (!m_command.isExplicit() && m_command.isNull() && !data.command.isNull())
        m_command = QString::fromUtf8(data.command);
    return m_command.value();
}

void IrcMessagePrivate::setCommand(const QString& command)
{
    m_command.setValue(command);
}

QStringList IrcMessagePrivate::params() const
{
    if (!m_params.isExplicit() && m_params.isNull() && !data.params.isEmpty()) {
        QStringList params;
        foreach (const QByteArray& param, data.params)
            params += QString::fromUtf8(param);
        m_params = params;
    }
    return m_params.value();
}

QString IrcMessagePrivate::param(int index) const
{
    return params().value(index);
}

void IrcMessagePrivate::setParams(const QStringList& params)
{
    m_params.setValue(params);
}

TagsRef IrcMessagePrivate::tags() const
{
    if (!m_tags.isExplicit() && m_tags.isNull() && !data.tags.empty()) {
        std::unordered_map<std::string_view, QString> tags;
        for (const auto &[k, v] : data.tags) {
            tags.insert_or_assign(k, QString::fromUtf8(v.data(), v.size()));
        }
        m_tags = tags;
    }
    return TagsRef(m_tags.value());
}

void IrcMessagePrivate::setTags(std::unordered_map<std::string_view, QString> tags)
{
    m_tags.mutate() = std::move(tags);
}

void IrcMessagePrivate::insertTag(std::string_view name, const QString &value)
{
    m_tags.mutate().insert_or_assign(name, value);
}

QByteArray IrcMessagePrivate::content() const
{
    if (m_prefix.isExplicit() || m_command.isExplicit() || m_params.isExplicit() || m_tags.isExplicit()) {
        QByteArray data;

        // format <tags>
        const auto &t = tags().raw();
        if (!t.empty()) {
            data += '@';
            bool first = true;
            for (const auto &[k, v] : t) {
                if (first) {
                    first = false;
                } else {
                    data += ';';
                }
                data += k;
                data += '=';
                data += v.toUtf8();
            }
            data += ' ';
        }

        // format <prefix>
        const QString p = prefix();
        if (!p.isEmpty())
            data += ':' + p.toUtf8() + ' ';

        // format <command>
        data += command().toUtf8();

        // format <params>
        foreach (const QString& param, params()) {
            data += ' ';
            if (param.isEmpty() || param.startsWith(QLatin1Char(':')) || param.contains(QLatin1Char(' ')))
                data += ':';
            data += param.toUtf8();
        }
        return data;
    }

    return data.content;
}

void IrcMessagePrivate::invalidate()
{
    m_nick.clear();
    m_ident.clear();
    m_host.clear();

    m_prefix.clear();
    m_command.clear();
    m_params.clear();
    m_tags.clear();
}

namespace
{

std::pair<QByteArrayView, QByteArrayView> splitOnce(QByteArrayView view, char c)
{
    auto idx = view.indexOf(c);
    if (idx <= 0) {
        return {view, {}};
    }
    return {view.sliced(0, idx), view.sliced(idx + 1)};
}

} // namespace

IrcMessageData IrcMessageData::fromData(const QByteArray &data)
{
    IrcMessageData message;
    message.content = data;

    // From RFC 1459:
    //  <message>  ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>
    //  <prefix>   ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]
    //  <command>  ::= <letter> { <letter> } | <number> <number> <number>
    //  <SPACE>    ::= ' ' { ' ' }
    //  <params>   ::= <SPACE> [ ':' <trailing> | <middle> <params> ]
    //  <middle>   ::= <Any *non-empty* sequence of octets not including SPACE
    //                 or NUL or CR or LF, the first of which may not be ':'>
    //  <trailing> ::= <Any, possibly *empty*, sequence of octets not including
    //                   NUL or CR or LF>

    // IRCv3.2 Message Tags
    //  <message> ::= ['@' <tags> <SPACE>] [':' <prefix> <SPACE> ] <command> <params> <crlf>
    //  <tags>    ::= <tag> [';' <tag>]*
    //  <tag>     ::= <key> ['=' <value>]
    //  <key>     ::= [ <vendor> '/' ] <sequence of letters, digits, hyphens (`-`)>
    //  <value>   ::= <sequence of any characters except NUL, BELL, CR, LF, semicolon (`;`) and SPACE>
    //  <vendor>  ::= <host>

    QByteArrayView process = data;

    // parse <tags>
    if (process.startsWith('@')) {
        process = process.sliced(1);
        auto spaceIdx = process.indexOf(' ');
        if (spaceIdx >= 0) {
            auto tags = process.left(spaceIdx);
            process = process.sliced(tags.size() + 1);

            while (!tags.empty()) {
                auto [fullTag, rest] = splitOnce(tags, ';');
                tags = rest;
                message.tags.emplace_back(splitOnce(fullTag, '='));
            }
        }
    }

    // parse <prefix>
    if (process.startsWith(':')) {
        message.prefix = process.left(process.indexOf(' ')).toByteArray();
        process = process.sliced(message.prefix.length() + 1);
    } else {
        // empty (not null)
        message.prefix = QByteArray("");
    }

    // parse <command>
    message.command = process.mid(0, process.indexOf(' ')).toByteArray();
    process = process.sliced(std::min(message.command.length() + 1, process.size()));

    // parse <params>
    while (!process.isEmpty()) {
        if (process.startsWith(':')) {
            process = process.sliced(1);
            message.params += process.toByteArray();
            process = {};
        } else {
            auto [param, rest] = splitOnce(process, ' ');
            process = rest;
            message.params += param.toByteArray();
        }
    }

    return message;
}

bool IrcMessagePrivate::parsePrefix(const QString& prefix, QString* nick, QString* ident, QString* host)
{
    const QString trimmed = prefix.trimmed();
    if (trimmed.contains(QLatin1Char(' ')))
        return false;

    const int len = trimmed.length();
    const int ex = trimmed.indexOf(QLatin1Char('!'));
    const int at = trimmed.indexOf(QLatin1Char('@'));

    if (ex == -1 && at == -1) {
        if (nick) *nick = trimmed;
    } else if (ex > 0 && at > 0 && ex + 1 < at && at < len - 1) {
        if (nick) *nick = trimmed.mid(0, ex);
        if (ident) *ident = trimmed.mid(ex + 1, at - ex - 1);
        if (host) *host = trimmed.mid(at + 1);
    } else if (ex > 0 && ex < len - 1 && at == -1) {
        if (nick) *nick = trimmed.mid(0, ex);
        if (ident) *ident = trimmed.mid(ex + 1);
    } else if (at > 0 && at < len - 1 && ex == -1) {
        if (nick) *nick = trimmed.mid(0, at);
        if (host) *host = trimmed.mid(at + 1);
    } else {
        return false;
    }
    return true;
}
#endif // IRC_DOXYGEN

IRC_END_NAMESPACE
