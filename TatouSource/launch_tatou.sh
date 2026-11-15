#!/usr/bin/env bash
set -euo pipefail

game_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd "$game_dir"

# Use the native X11 path under XWayland, including Steam Game Mode, and avoid
# optional injected Vulkan layers interfering with the game's swapchain.
export SDL_VIDEODRIVER=x11
export DISABLE_LSFGVK=1
export DISABLE_MAKO=1
export DISABLE_MANGOHUD=1
export DISABLE_GAMESCOPE_WSI=1

mkdir -p logs
exec ./Tatou "$@" >logs/rehaunted.log 2>&1
