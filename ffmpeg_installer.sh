#!/usr/bin/env bash
# ------------------------------------------------------------------
# FFmpeg + Extensions (AOM, SVT‑AV1, libdav1d, VMAF) bauen
# ------------------------------------------------------------------
set -euo pipefail          # sicherer Modus

#############################
# 1. System‑Pakete installieren
#############################
echo "=== Schritt 1: Grundpakete installieren ==="
sudo apt-get update -qq
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    autoconf automake build-essential cmake git-core libass-dev libfreetype6-dev \
    libgnutls28-dev libmp3lame-dev libsdl2-dev libtool libva-dev libvdpau-dev \
    libvorbis-dev libxcb1-dev libxcb-shm0-dev libxcb-xfixes0-dev meson ninja-build \
    pkg-config texinfo wget yasm zlib1g-dev nasm libx264-dev libx265-dev libnuma-dev \
    libvpx-dev libfdk-aac-dev libopus-dev libunistring-dev libdav1d-dev

# AOM, SVT‑AV1 und VMAF brauchen die oben installierten Bibliotheken
# (z. B. libnuma-dev, zlib1g-dev, etc.)

#############################
# 2. Verzeichnisse anlegen
#############################
echo "=== Schritt 2: Arbeitsverzeichnisse anlegen ==="
mkdir -p ~/ffmpeg_sources ~/bin

#############################
# 3. AOM bauen
#############################
echo "=== Schritt 3: AOM (libaom) bauen ==="
cd ~/ffmpeg_sources
git -C aom pull 2>/dev/null || git clone --depth 1 https://aomedia.googlesource.com/aom
mkdir -p aom_build && cd aom_build
cmake -G "Unix Makefiles" \
      -DCMAKE_INSTALL_PREFIX="$HOME/ffmpeg_build" \
      -DENABLE_TESTS=OFF \
      -DENABLE_NASM=ON \
      ../aom
make -j$(nproc)
sudo make install

#############################
# 4. SVT‑AV1 bauen
#############################
echo "=== Schritt 4: SVT-AV1 bauen ==="
cd ~/ffmpeg_sources
git -C SVT-AV1 pull 2>/dev/null || git clone https://gitlab.com/AOMediaCodec/SVT-AV1.git
mkdir -p SVT-AV1/build && cd SVT-AV1/build
cmake -G "Unix Makefiles" \
      -DCMAKE_INSTALL_PREFIX="$HOME/ffmpeg_build" \
      -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_DEC=OFF \
      -DBUILD_SHARED_LIBS=OFF \
      ..
make -j$(nproc)
sudo make install

#############################
# 5. VMAF bauen
#############################
echo "=== Schritt 5: VMAF (libvmaf) bauen ==="
cd ~/ffmpeg_sources
git clone --depth 1 https://github.com/Netflix/vmaf vmaf-master || true
mkdir -p vmaf-master/libvmaf/build && cd vmaf-master/libvmaf/build
meson setup \
    -Denable_tests=false \
    -Denable_docs=false \
    --buildtype=release \
    --default-library=static \
    '../' \
    --prefix="$HOME/ffmpeg_build" \
    --bindir="$HOME/bin" \
    --libdir="$HOME/ffmpeg_build/lib"
ninja
sudo ninja install

#############################
# 6. FFmpeg bauen
#############################
echo "=== Schritt 6: FFmpeg bauen ==="
cd ~/ffmpeg_sources
wget -O ffmpeg-snapshot.tar.bz2 https://ffmpeg.org/releases/ffmpeg-snapshot.tar.bz2
tar xjvf ffmpeg-snapshot.tar.bz2
cd ffmpeg

PKG_CONFIG_PATH="$HOME/ffmpeg_build/lib/pkgconfig" \
./configure \
    --prefix="$HOME/ffmpeg_build" \
    --pkg-config-flags="--static" \
    --extra-cflags="-I$HOME/ffmpeg_build/include" \
    --extra-ldflags="-L$HOME/ffmpeg_build/lib" \
    --extra-libs="-lpthread -lm" \
    --bindir="$HOME/bin" \
    --enable-gpl \
    --enable-gnutls \
    --enable-nonfree \
    --enable-libass \
    --enable-libfdk-aac \
    --enable-libfreetype \
    --enable-libmp3lame \
    --enable-libopus \
    --enable-libsvtav1 \
    --enable-libdav1d \
    --enable-libvorbis \
    --enable-libvpx \
    --enable-libx264 \
    --enable-libx265 \
    --enable-libaom

make -j$(nproc)
sudo make install
hash -r   # Shell‑Cache zurücksetzen

#############################
# 7. Befehle ins System einbinden
#############################
echo "=== Schritt 7: Binärdateien in /usr/local/bin kopieren ==="
sudo cp ~/bin/* /usr/local/bin

echo "FFmpeg-Build fertig! Testen Sie mit 'ffmpeg -version'."
