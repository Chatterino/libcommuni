/*
  Copyright (C) 2008-2026 The Communi Project

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

#ifndef IRCTAGSREF_H
#define IRCTAGSREF_H

#include <IrcGlobal>
#include <QVariantMap>

IRC_BEGIN_NAMESPACE

/// A reference/view type for tags of an IRC message.
///
/// This should be passed by value, similar to a string view.
/// Conceptually, this is a typedef to `const MapType &`.
struct IRC_GSL_POINTER TagsRef
{
    using MapType = std::unordered_map<std::string_view, QString>;

    /// Create a reference to a map.
    ///
    /// The created reference must not outlive `map`.
    explicit TagsRef(const MapType &map IRC_LIFETIMEBOUND);

    /// Get the raw map pointer.
    ///
    /// Avoid using this - use the helper functions in this type.
    const MapType &raw() const;

    /// Get a tag by its name. If it doesn't exist, `std::nullopt` is returned.
    [[nodiscard]] std::optional<QString> get(std::string_view tag) const;

    /// Get a tag by its name or a fallback. If it doesn't exist in the map, use `other`.
    [[nodiscard]] QString getOr(std::string_view tag, const QString &other) const;

    /// Get a tag by its name or an empty string if it doesn't exist.
    [[nodiscard]] QString getOrEmpty(std::string_view tag) const;

    /// Check if a tag was specified.
    [[nodiscard]] bool has(std::string_view tag) const;

  private:
    // Stored as a pointer instead of a reference to generate the implicit copy/move ctors.
    // This is never null.
    const MapType *storage;
};

IRC_END_NAMESPACE

#endif // IRCTAGSREF_H
