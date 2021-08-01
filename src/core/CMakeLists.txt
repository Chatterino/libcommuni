# ------------------------------------------------------------------------------------------------#
#  Core
# ------------------------------------------------------------------------------------------------#
include(GNUInstallDirs)

project(Core)

set(CONV_HEADERS
        ${LibCommuni_INCLUDE_DIR}/IrcCore/Irc
        ${LibCommuni_INCLUDE_DIR}/IrcCore/IrcCommand
        ${LibCommuni_INCLUDE_DIR}/IrcCore/IrcCommandFilter
        ${LibCommuni_INCLUDE_DIR}/IrcCore/IrcConnection
        ${LibCommuni_INCLUDE_DIR}/IrcCore/IrcCore
        ${LibCommuni_INCLUDE_DIR}/IrcCore/IrcGlobal
        ${LibCommuni_INCLUDE_DIR}/IrcCore/IrcMessage
        ${LibCommuni_INCLUDE_DIR}/IrcCore/IrcMessageFilter
        ${LibCommuni_INCLUDE_DIR}/IrcCore/IrcNetwork
        ${LibCommuni_INCLUDE_DIR}/IrcCore/IrcProtocol
        )

set(PUB_HEADERS
        ${LibCommuni_INCLUDE_DIR}/IrcCore/irc.h
        ${LibCommuni_INCLUDE_DIR}/IrcCore/irccommand.h
        ${LibCommuni_INCLUDE_DIR}/IrcCore/ircconnection.h
        ${LibCommuni_INCLUDE_DIR}/IrcCore/irccore.h
        ${LibCommuni_INCLUDE_DIR}/IrcCore/ircfilter.h
        ${LibCommuni_INCLUDE_DIR}/IrcCore/ircglobal.h
        ${LibCommuni_INCLUDE_DIR}/IrcCore/ircmessage.h
        ${LibCommuni_INCLUDE_DIR}/IrcCore/ircnetwork.h
        ${LibCommuni_INCLUDE_DIR}/IrcCore/ircprotocol.h
        )

set(PRIV_HEADERS
        ${LibCommuni_INCLUDE_DIR}/IrcCore/irccommand_p.h
        ${LibCommuni_INCLUDE_DIR}/IrcCore/ircconnection_p.h
        ${LibCommuni_INCLUDE_DIR}/IrcCore/ircdebug_p.h
        ${LibCommuni_INCLUDE_DIR}/IrcCore/ircmessage_p.h
        ${LibCommuni_INCLUDE_DIR}/IrcCore/ircmessagecomposer_p.h
        ${LibCommuni_INCLUDE_DIR}/IrcCore/ircmessagedecoder_p.h
        ${LibCommuni_INCLUDE_DIR}/IrcCore/ircnetwork_p.h
        )

set(SOURCE_FILES
        ${CMAKE_CURRENT_LIST_DIR}/irc.cpp
        ${CMAKE_CURRENT_LIST_DIR}/irccommand.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ircconnection.cpp
        ${CMAKE_CURRENT_LIST_DIR}/irccore.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ircfilter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ircmessage.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ircmessage_p.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ircmessagecomposer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ircmessagedecoder.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ircnetwork.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ircprotocol.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ircmessagedecoder_none.cpp
        #${LibCommuni_SOURCE_DIR}/3rdparty/mozilla/rdf_utils.c
        )

# Skip running moc for ircfilter.cpp as it does not need any moccing, but automoc finds a Q_OBJECT line in the comment in ircfilter.cpp
set_property(SOURCE ircfilter.cpp PROPERTY SKIP_AUTOMOC ON)
# We cannot skip running moc for irccommand.cpp because it has IRC_OBJECT defined in its header file and it needs moccing, however, there's a Q_OBJECT comment in the irccommand.cpp file which I've had to "break" to make this work
# set_property(SOURCE irccommand.cpp PROPERTY SKIP_AUTOMOC ON)

add_library(${PROJECT_NAME} STATIC ${PRIV_HEADERS} ${PUB_HEADERS} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt${MAJOR_QT_VERSION}::Core
    Qt${MAJOR_QT_VERSION}::Network
    )
if (BUILD_WITH_QT6)
    target_link_libraries(${PROJECT_NAME} PUBLIC
        Qt${MAJOR_QT_VERSION}::Core5Compat
        )
endif()
target_include_directories(${PROJECT_NAME} PUBLIC "${LibCommuni_INCLUDE_DIR}/IrcCore/")
target_compile_definitions(${PROJECT_NAME} PUBLIC IRC_STATIC IRC_NAMESPACE=Communi)

install(FILES ${PUB_HEADERS} ${CONV_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/IrcCore/)
install(TARGETS ${PROJECT_NAME} LIBRARY ARCHIVE)
