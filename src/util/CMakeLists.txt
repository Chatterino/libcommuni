# ------------------------------------------------------------------------------------------------#
#  Util
# ------------------------------------------------------------------------------------------------#

project(Util)

set(CONV_HEADERS
        ${LibCommuni_INCLUDE_DIR}/IrcUtil/IrcCommandParser
        ${LibCommuni_INCLUDE_DIR}/IrcUtil/IrcCommandQueue
        ${LibCommuni_INCLUDE_DIR}/IrcUtil/IrcCompleter
        ${LibCommuni_INCLUDE_DIR}/IrcUtil/IrcLagTimer
        ${LibCommuni_INCLUDE_DIR}/IrcUtil/IrcPalette
        ${LibCommuni_INCLUDE_DIR}/IrcUtil/IrcTextFormat
        ${LibCommuni_INCLUDE_DIR}/IrcUtil/IrcUtil
        )

set(PUB_HEADERS
        ${LibCommuni_INCLUDE_DIR}/IrcUtil/irccommandparser.h
        ${LibCommuni_INCLUDE_DIR}/IrcUtil/irccommandqueue.h
        ${LibCommuni_INCLUDE_DIR}/IrcUtil/irccompleter.h
        ${LibCommuni_INCLUDE_DIR}/IrcUtil/irclagtimer.h
        ${LibCommuni_INCLUDE_DIR}/IrcUtil/ircpalette.h
        ${LibCommuni_INCLUDE_DIR}/IrcUtil/irctextformat.h
        ${LibCommuni_INCLUDE_DIR}/IrcUtil/ircutil.h
        )

set(PRIV_HEADERS
        ${LibCommuni_INCLUDE_DIR}/IrcUtil/irccommandparser_p.h
        ${LibCommuni_INCLUDE_DIR}/IrcUtil/irccommandqueue_p.h
        ${LibCommuni_INCLUDE_DIR}/IrcUtil/irclagtimer_p.h
        ${LibCommuni_INCLUDE_DIR}/IrcUtil/irctoken_p.h
        )

set(SOURCE_FILES
        ${CMAKE_CURRENT_LIST_DIR}/irccommandparser.cpp
        ${CMAKE_CURRENT_LIST_DIR}/irccommandqueue.cpp
        ${CMAKE_CURRENT_LIST_DIR}/irccompleter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/irclagtimer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ircpalette.cpp
        ${CMAKE_CURRENT_LIST_DIR}/irctextformat.cpp
        ${CMAKE_CURRENT_LIST_DIR}/irctoken.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ircutil.cpp
        )

add_library(${PROJECT_NAME} STATIC ${PUB_HEADERS} ${PRIV_HEADERS} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt${MAJOR_QT_VERSION}::Core
    Qt${MAJOR_QT_VERSION}::Network
    )
if (BUILD_WITH_QT6)
    target_link_libraries(${PROJECT_NAME} PUBLIC
        Qt${MAJOR_QT_VERSION}::Core5Compat
        )
endif()
target_include_directories(${PROJECT_NAME} PUBLIC "${LibCommuni_INCLUDE_DIR}/IrcUtil/" "${LibCommuni_INCLUDE_DIR}/IrcModel/" "${LibCommuni_INCLUDE_DIR}/IrcCore/")
target_compile_definitions(${PROJECT_NAME} PUBLIC IRC_STATIC IRC_NAMESPACE=Communi)

install(FILES ${PUB_HEADERS} ${CONV_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/IrcUtil/)
install(TARGETS ${PROJECT_NAME} LIBRARY ARCHIVE)
