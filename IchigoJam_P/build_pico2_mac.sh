#!/bin/bash
set -e

cd "$(dirname "$0")"

export PICO_SDK_PATH="$PWD/pico-sdk-rp2350"
export PICO_EXTRAS_PATH="$PWD/pico-extras-rp2350"

rm -rf build-pico2

cmake -S . -B build-pico2 \
  -G Ninja \
  -DPICO_BOARD=pico2

cmake --build build-pico2
