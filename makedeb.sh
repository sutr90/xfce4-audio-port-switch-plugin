#!/bin/bash
PROJECT_NAME="xfce4-audio-port-switch-plugin"
VERSION="1.0-1"
DIR="${PROJECT_NAME}_$VERSION"

rm -rf $DIR
mkdir $DIR
mkdir -p $DIR/usr/share/xfce4/panel-plugins
mkdir -p $DIR/usr/share/xfce4/port-switch
mkdir -p $DIR/usr/lib/x86_64-linux-gnu/xfce4/panel-plugins
mkdir -p $DIR/DEBIAN

cp ./headphones.svg $DIR/usr/share/xfce4/port-switch
cp ./speaker.svg $DIR/usr/share/xfce4/port-switch
cp ./sample.desktop $DIR/usr/share/xfce4/panel-plugins
cp ./libsample.so $DIR/usr/lib/x86_64-linux-gnu/xfce4/panel-plugins/libsample.so
cp ./control $DIR/DEBIAN

dpkg-deb --build $DIR
