# qt-wayland-decorations-bradient-mkii
Fork from https://github.com/qt/qtwayland/blob/6.6.3/src/plugins/decorations/bradient/main.cpp

Scale the window decorations within HiDPI.

Support both Qt5 and Qt6 now.  
  - Build with Qt5: `cmake -B build5 && cmake --build build5`
  - Build with Qt6: `cmake -B build6 -DUSE_QT6=1 && cmake --build build6`
