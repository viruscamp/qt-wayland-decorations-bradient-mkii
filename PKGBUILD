# Maintainer: viruscamp <viruscamp@gmail.com>

_pkgbase=qt-wayland-decorations-bradient-mkii
pkgbase=$_pkgbase-git
pkgname=(qt5-wayland-decorations-bradient-mkii-git qt6-wayland-decorations-bradient-mkii-git)
pkgver=r12.d16d367
pkgrel=1
pkgdesc='Qt decoration plugin with HiDPI support base on official plugin bradient'
arch=('x86_64')
url='https://github.com/viruscamp/qt-wayland-decorations-bradient-mkii'
license=(LGPL3.0)
makedepends=(git make cmake qt5-base qt5-wayland qt6-base qt6-wayland)
source=("git+https://github.com/viruscamp/qt-wayland-decorations-bradient-mkii.git")
sha256sums=('SKIP')

pkgver() {
  cd "$_pkgbase"
  printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short=7 HEAD)"
}

build() {
  cmake -B build-qt5 -S $_pkgbase \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DHAS_QT6_SUPPORT=true \
        -DUSE_QT6=false
  make -C build-qt5

  cmake -B build-qt6 -S $_pkgbase \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DUSE_QT6=true
  make -C build-qt6
}

package_qt5-wayland-decorations-bradient-mkii-git() {
  provides=('qt5-wayland-decorations-bradient-mkii')
  pkgdesc='Qt5 decoration plugin with HiDPI support base on official plugin bradient'
  QT_VER=$(pacman -Sp qt5-wayland --print-format '%v')
  depends=(qt5-base qt5-wayland=$QT_VER)

  DESTDIR="$pkgdir" make -C build-qt5 install
}

package_qt6-wayland-decorations-bradient-mkii-git() {
  provides=('qt6-wayland-decorations-bradient-mkii')
  pkgdesc='Qt6 decoration plugin with HiDPI support base on official plugin bradient'
  QT_VER=$(pacman -Sp qt6-wayland --print-format '%v')
  depends=(qt6-base qt6-wayland=$QT_VER)

  DESTDIR="$pkgdir" make -C build-qt6 install
}
