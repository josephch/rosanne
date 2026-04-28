#!/bin/sh

mkdir -p Rosanne.AppDir/usr/share/images
cp include/images/main_icon_32.xpm Rosanne.AppDir/


cd xrs/cards/
unzip -o cards.zip
wxrc cards.xrc -o cards.xrs
cd ../../
cp xrs/cards/cards.xrs Rosanne.AppDir/

cd include/images
cp ../../xrs/gui/gui.xrc .
wxrc gui.xrc -o gui.xrs
cd ../../
cp include/images/gui.xrs Rosanne.AppDir/

~/Downloads/appimagetool-x86_64.AppImage Rosanne.AppDir/
