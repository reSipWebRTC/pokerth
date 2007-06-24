#!/bin/bash
# Author: Erhard List (dunkanx@pokerth.net) Version: 1.0

# ATTENTION: this is for MAC only!
#
# This script integrates the necessary frameworks into the binary.
#
# The integration of Qt is optional (commandlineswitch --without-qt) as you can save 30MB 
# of binary-size if you leave Qt out.
# Using such a "small" version needs to install qt 4.2.3 on the mac seperatly as a framework 
# (see http://trolltech.com/developer/downloads/qt/mac)

QT_FW_PATH="/Library/Frameworks"
SDL_FW_PATH="/Library/Frameworks"
APPLICATION="./bin/pokerth.app"
BINARY="$APPLICATION/Contents/MacOs/pokerth"

# strip binary
strip $BINARY

# create framework-path
BINARY_FW_PATH="$APPLICATION/Contents/Frameworks"
mkdir $BINARY_FW_PATH

# integrate SDL-frameworks into binary
cp -R $SDL_FW_PATH/SDL.framework $BINARY_FW_PATH
cp -R $SDL_FW_PATH/SDL_mixer.framework $BINARY_FW_PATH

# integrate Qt-frameworks into binary

if [ "$1" != "--without-qt" ] ; then
	
	cp -R $QT_FW_PATH/QtCore.framework $BINARY_FW_PATH
	cp -R $QT_FW_PATH/QtGui.framework $BINARY_FW_PATH
	
	# redirect binary to use integrated frameworks
	QTCORE="QtCore.framework/Versions/4.0/QtCore"
	QTGUI="QtGui.framework/Versions/4.0/QtGui"
	install_name_tool -id @executable_path/../Frameworks/$QTCORE $BINARY_FW_PATH/$QTCORE
	install_name_tool -id @executable_path/../Frameworks/$QTGUI $BINARY_FW_PATH/$QTGUI
	QTCORE_LINK=$(otool -L $BINARY | grep QtCore | cut -d"(" -f1 | cut -f2)
	QTGUI_LINK=$(otool -L $BINARY | grep QtGui | cut -d"(" -f1 | cut -f2)
	install_name_tool -change $QTCORE_LINK @executable_path/../Frameworks/$QTCORE $BINARY
	install_name_tool -change $QTGUI_LINK @executable_path/../Frameworks/$QTGUI $BINARY
	QTCORE_LINK=$(otool -L $BINARY_FW_PATH/$QTGUI | grep QtCore | cut -d"(" -f1 | cut -f2)
	install_name_tool -change $QTCORE_LINK @executable_path/../Frameworks/$QTCORE $BINARY_FW_PATH/$QTGUI
	
fi