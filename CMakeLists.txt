cmake_minimum_required(VERSION 2.8.4)
project(slackapp)
FIND_PACKAGE(Qt4 REQUIRED)

SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTWEBKIT TRUE)
INCLUDE(${QT_USE_FILE})

ADD_DEFINITIONS(${QT_DEFINITIONS})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(slackapp_SOURCES main.cpp slackapp.cpp filedownloader.cpp)
set(slackapp_HEADERS slackapp.h filedownloader.h)
set(slackapp_FORMS form.ui)
set(slackapp_RESOURCES resources.qrc)

QT4_WRAP_CPP(slackapp_HEADERS_MOC ${slackapp_HEADERS})
QT4_WRAP_UI(slackapp_FORM_HEADERS ${slackapp_FORMS})
QT4_ADD_RESOURCES(slackapp_RESOURCES_RCC ${slackapp_RESOURCES})

add_executable(slackapp ${slackapp_SOURCES} ${slackapp_HEADERS_MOC} ${slackapp_FORM_HEADERS} ${slackapp_RESOURCES_RCC})
TARGET_LINK_LIBRARIES(slackapp ${QT_LIBRARIES})