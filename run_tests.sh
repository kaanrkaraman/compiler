#!/usr/bin/env bash

set -e

echo "[INFO] Cleaning previous build..."
rm -rf build
mkdir -p build
cd build

echo "[INFO] Detecting OS and CPU cores..."
OS=$(uname -s)

if command -v nproc >/dev/null 2>&1; then
    CORES=$(nproc)
elif [[ "$OS" == "Darwin" ]]; then
    CORES=$(sysctl -n hw.logicalcpu)
elif [[ "$OS" == "Linux" ]]; then
    CORES=$(grep -c ^processor /proc/cpuinfo)
else
    echo "[WARN] Unable to detect CPU cores. Defaulting to 2."
    CORES=2
fi

echo "[INFO] Configuring CMake..."
cmake ..

echo "[INFO] Building with $CORES cores..."
cmake --build . -- -j"$CORES"

echo "[INFO] Running tests..."
ctest --output-on-failure