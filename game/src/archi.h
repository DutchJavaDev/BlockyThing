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

int teleX = 0;
int teleY = 0;

typedef struct
{
	char Name[28];
	float x;
	float y;
	int width;
	int height;
} WorldObject;

WorldObject Objects[maxWorldObjects];
int nextObjectIndex = 0;

static void LoadWorld(ecs_world_t* world, FILE* worldFile)
{
	// ????
	char lineBuffer[maxLineBuffer];
	memset(&lineBuffer[0], '\0', maxLineBuffer);
	memset(&Objects[0], '\0', maxWorldObjects);
	// Read file line by line
	// line 0: worldWidth, worldHeight, tileWidth, tileHeight
	// rest is of the lines is about the tiles
	int line = 0;
	if (fgets(lineBuffer, maxLineBuffer, worldFile) != NULL)
	{
		int rtn = sscanf(lineBuffer, "%d %d %d %d", &worldWidth, &worldHeight, &tileWidth, &tileHeight);
		worldWidth = worldWidth * tileWidth;
		worldHeight = worldHeight * tileHeight;

		while (1)
		{
			if (fgets(lineBuffer, maxLineBuffer, worldFile) == NULL)
			{
				break;
			}

			if (strcmp("object_data", lineBuffer))
			{
				// Keep reading until end of map objects
				while (1)
				{
					if (fgets(lineBuffer, maxLineBuffer, worldFile) == NULL)
					{
						break;
					}

					if (strcmp("object_data_end\n", lineBuffer) >= 0)
					{
						break;
					}

					// Load into object struct
					Objects[nextObjectIndex] = (WorldObject){ 1 };

					int rtn = sscanf(lineBuffer, "%s %f %f %d %d", &Objects[nextObjectIndex].Name, &Objects[nextObjectIndex].x,
						&Objects[nextObjectIndex].y, &Objects[nextObjectIndex].width, &Objects[nextObjectIndex].height);

					nextObjectIndex++;
				}
			}

			if (fgets(lineBuffer, maxLineBuffer, worldFile) == NULL)
			{
				break;
			}

			if (strcmp("tile_data", lineBuffer))
			{
				while (1)
				{
					if (fgets(lineBuffer, maxLineBuffer, worldFile) == NULL)
					{
						break;
					}

					if (strcmp("tile_data_end", lineBuffer) == 0)
					{
						break;
					}

					int tileId;
					float tileX;
					float tileY;

					int rtn = sscanf(lineBuffer, "%d %f %f", &tileId, &tileX, &tileY);

					Entityies[nextEntityIndexCount] = ecs_new_id(world);

					tileX = tileX * tileWidth;
					tileY = tileY * tileHeight;

					if (tileId == 0)
					{
						ecs_set(world, Entityies[nextEntityIndexCount], StaticBody, { 0 });
					}

					ecs_set(world, Entityies[nextEntityIndexCount], WorldPosition, { tileX, tileY });
					ecs_set(world, Entityies[nextEntityIndexCount], WorldTile, { tileId });
					ecs_set(world, Entityies[nextEntityIndexCount], Shape, { RECTANGLE_SHAPE,tileWidth,tileHeight });

					nextEntityIndexCount++;
				}
			}

		}

		// Load objects
		// Player etc
		for (int i = 0; i < maxWorldObjects; i++)
		{
			WorldObject* obj = &Objects[i];

			if (strcmp("player_spawn", obj->Name) >= 0)
			{
				Entityies[nextEntityIndexCount] = ecs_new_id(world);
				ecs_set(world, Entityies[nextEntityIndexCount], WorldPosition, { obj->x, obj->y });
				ecs_set(world, Entityies[nextEntityIndexCount], Player, { 0 });
				ecs_set(world, Entityies[nextEntityIndexCount], DynamicBody, { 0 });
				ecs_set(world, Entityies[nextEntityIndexCount], Velocity, { 0,0 });
				ecs_set(world, Entityies[nextEntityIndexCount], Shape, { RECTANGLE_SHAPE,tileWidth,tileHeight });
				ecs_set(world, Entityies[nextEntityIndexCount], WorldTile, { 0 });
				teleX = obj->x;
				teleY = obj->y;
				nextEntityIndexCount++;
			}

			if (strcmp("col", obj->Name) >= 0)
			{
				Entityies[nextEntityIndexCount] = ecs_new_id(world);
				ecs_set(world, Entityies[nextEntityIndexCount], StaticBody, { 0 });
				ecs_set(world, Entityies[nextEntityIndexCount], WorldPosition, { obj->x, obj->y });
				ecs_set(world, Entityies[nextEntityIndexCount], WorldTile, { 0 });
				ecs_set(world, Entityies[nextEntityIndexCount], Shape, { RECTANGLE_SHAPE,obj->width, obj->height });
				nextEntityIndexCount++;
			}
		}
	}
}
