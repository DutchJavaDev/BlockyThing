#pragma once
#include "stdio.h"
#include "stdlib.h"
#include "raylib.h"
#include "raymath.h"
#define CUTE_TILED_IMPLEMENTATION
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif
#include "flecs.h"
#include "ecs_components.h"
#include "ecs_systems.h"
#include "tiles.h"
#include "game_init.h"
#include "player.h"
#include "collision.h"
