#include <nwge/engine.hpp>
#include "states.hpp"

s32 main([[maybe_unused]] s32 argc, [[maybe_unused]] CStr *argv) {
  nwge::startPtr(mine::getMineFieldState(), {
    .appName = "Minesweeper",
    .windowResizable = false,
    .windowBorderless = true,
    .windowAspectRatio = {4, 3}
  });
  return 0;
}
