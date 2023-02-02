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

#define maxLineBuffer 32
#define maxWorldObjects 32

// If LoadWorld fails then there is no game to be played
int worldWidth = 0;
int worldHeight = 0;
int tileWidth = 0;
int tileHeight = 0;

typedef struct
{
	char Name;
	int x;
	int y;
} WorldObject;

static void LoadWorld(ecs_world_t* world, FILE* worldFile)
{
	// ????
	char lineBuffer[maxLineBuffer];
	memset(&lineBuffer[0], '\0', maxLineBuffer);
	// Read file line by line
	// line 0: worldWidth, worldHeight, tileWidth, tileHeight
	// rest is of the lines is about the tiles
	int line = 0;
	if (fgets(lineBuffer, maxLineBuffer, worldFile) != NULL)
	{
		int rtn = sscanf(lineBuffer, "%d,%d,%d,%d", &worldWidth, &worldHeight, &tileWidth, &tileHeight);
		worldWidth = worldWidth * tileWidth;
		worldHeight = worldHeight * tileHeight;

		while (1)
		{
			if (fgets(lineBuffer, maxLineBuffer, worldFile) == NULL)
			{
				break;
			}

			if (strcmp("map_obj", lineBuffer))
			{
				// Keep reading until end of map objects
				while (1)
				{
					if (fgets(lineBuffer, maxLineBuffer, worldFile) == NULL)
					{
						break;
					}

					if (strcmp("map_obj_end", lineBuffer) == 0)
					{
						break;
					}


					// Load into object struct
					char objName[16];
					int x;
					int y;

					sscanf(lineBuffer, "%s %d %d", &objName, &x, &y);
				}
			}



		}

		int tileType;
		int tileX;
		int tileY;
		sscanf(lineBuffer, "%d,%d,%d", &tileType, &tileX, &tileY);

		Entityies[nextEntityIndexCount] = ecs_new_id(world);

		tileX = tileX * tileWidth;
		tileY = tileY * tileHeight;

		if (line == 1)
		{
			ecs_set(world, Entityies[nextEntityIndexCount], WorldPosition, { tileX, tileY });
			ecs_set(world, Entityies[nextEntityIndexCount], Player, { 0 });
			ecs_set(world, Entityies[nextEntityIndexCount], DynamicBody, { 0 });
			ecs_set(world, Entityies[nextEntityIndexCount], Velocity, { 0,0 });
			ecs_set(world, Entityies[nextEntityIndexCount], Shape, { RECTANGLE_SHAPE,tileWidth,tileHeight });
			ecs_set(world, Entityies[nextEntityIndexCount], WorldTile, { tileType });
		}
		else
		{
			if (tileType == 0)
			{
				ecs_set(world, Entityies[nextEntityIndexCount], StaticBody, { 0 });
			}
			ecs_set(world, Entityies[nextEntityIndexCount], WorldPosition, { tileX, tileY });
			ecs_set(world, Entityies[nextEntityIndexCount], WorldTile, { tileType });
			ecs_set(world, Entityies[nextEntityIndexCount], Shape, { RECTANGLE_SHAPE,tileWidth,tileHeight });
		}

		nextEntityIndexCount++;
	}
}
