# -*- mode: makefile -*-
######################################################################
#  Copyright 2013 Mats Sjöberg
#  
#  This file is part of the Pumpa programme.
#
#  Pumpa is free software: you can redistribute it and/or modify it
#  under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  Pumpa is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with Pumpa.  If not, see <http://www.gnu.org/licenses/>.
######################################################################

TEMPLATE = app
TARGET = pumpa
RESOURCES += pumpa.qrc
OBJECTS_DIR = obj

QT += core gui network

# Enable for gdb debug info
CONFIG += debug

# Use aspell
CONFIG += aspell

# Additions for Qt 4
lessThan(QT_MAJOR_VERSION, 5) {
  message("Configuring for Qt 4")
  LIBS += -lqjson
}

# Additions for Qt 5
greaterThan(QT_MAJOR_VERSION, 4) { 
  message("Configuring for Qt 5")
  QT += widgets
  DEFINES += QT5
}

# Optional spell checking support with libaspell
exists( /usr/lib/libaspell* ) {
  message("Using aspell")
  LIBS += -laspell
  DEFINES += USE_ASPELL
}

######################################################################
# Main sources 
######################################################################

INCLUDEPATH += src
VPATH       += src

OBJECT_HEADERS = pumpapp.h qactivitystreams.h collectionwidget.h json.h \
	messagewindow.h messageedit.h fancyhighlighter.h qaspell.h \
	activitywidget.h objectwidget.h actorwidget.h filedownloader.h \
	richtextlabel.h

HEADERS += $$OBJECT_HEADERS
SOURCES += main.cpp
SOURCES += $$replace(OBJECT_HEADERS, \\.h, .cpp)


######################################################################
# kQOAuth sources
######################################################################

INCLUDEPATH += src/kQOAuth
VPATH       += src/kQOAuth

PUBLIC_HEADERS += kqoauthmanager.h \
                  kqoauthrequest.h \
                  kqoauthrequest_1.h \
                  kqoauthrequest_xauth.h \
                  kqoauthglobals.h 

PRIVATE_HEADERS +=  kqoauthrequest_p.h \
                    kqoauthmanager_p.h \
                    kqoauthauthreplyserver.h \
                    kqoauthauthreplyserver_p.h \
                    kqoauthutils.h \
                    kqoauthrequest_xauth_p.h

HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS 

SOURCES += \
    kqoauthmanager.cpp \
    kqoauthrequest.cpp \
    kqoauthutils.cpp \
    kqoauthauthreplyserver.cpp \
    kqoauthrequest_1.cpp \
    kqoauthrequest_xauth.cpp

