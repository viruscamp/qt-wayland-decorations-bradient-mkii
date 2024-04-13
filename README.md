# qt-wayland-decorations-bradient-mkii
Fork from https://github.com/qt/qtwayland/blob/6.6.3/src/plugins/decorations/bradient/main.cpp

Scale the window decorations within HiDPI.

Support both Qt5 and Qt6 now.  
  - Build with Qt5:  
    ```sh
    cmake -B build5
    cmake --build build5
    sudo cp build5/libbradient-mkii.so /usr/lib/qt/plugins/wayland-decoration-client/
    ```
  - Build with Qt6:  
    ```sh
    cmake -B build6 -DUSE_QT6=1
    cmake --build build6
    sudo cp build6/libbradient-mkii.so /usr/lib/qt6/plugins/wayland-decoration-client/
    ```

Usage:
  - `QT_WAYLAND_DECORATION=bradient-mkii featherpad`
  - Or add `export QT_WAYLAND_DECORATION=bradient-mkii` to you `~/.bash_profile`
