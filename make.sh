#!/bin/bash
#gcc -Wall -shared -o libsample.so -fPIC *.c `pkg-config --cflags --libs libxfce4panel-2.0` `pkg-config --cflags --libs libxfce4ui-1` `pkg-config --cflags --libs gtk+-3.0` `pkg-config --cflags --libs librsvg-2.0`
cp /home/jose/ClionProjects/xfce4-audio-port-switch-plugin/build/Debug/libport-switch.so /usr/lib/x86_64-linux-gnu/xfce4/panel-plugins/libsample.so
cp sample.desktop /usr/share/xfce4/panel-plugins/