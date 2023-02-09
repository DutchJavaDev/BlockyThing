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
	//memset(&Objects[0], '\0', maxWorldObjects);
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

					if (strcmp("object_data_end\n", lineBuffer) == 0)
					{
						break;
					}

					// Load into object struct
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

					if (strcmp("tile_data_end\n", lineBuffer) == 0)
					{
						break;
					}

					int layerId;
					int tileId;
					float tileX;
					float tileY;
					int tileTextureX;
					int tileTextureY;

					int yOffset;

					int rtn = sscanf(lineBuffer, "%d %d %f %f %d %d", &layerId, &tileId, &tileX, &tileY, &tileTextureX, &tileTextureY);

					if (rtn == 0)
					{
						break;
					}

					Entityies[nextEntityIndexCount] = ecs_new_id(world);

					tileX = tileX * tileWidth;
					tileY = tileY * tileHeight;

					switch (tileId)
					{
					case 18:
					case 19:
					case 20:
					case 27:
					case 28:
						ecs_set(world, Entityies[nextEntityIndexCount], StaticBody, { tileId, tileX + tileWidth / 2, tileY + tileHeight / 2, tileWidth, tileHeight });
						break;

					case 15:
					case 16:
					case 17:
					case 36:
					case 37:
					case 38:
						yOffset = tileHeight * 0.85;
						ecs_set(world, Entityies[nextEntityIndexCount], StaticBody,
							{
								tileId,
								tileX + tileWidth / 2,
								tileY + yOffset,
								tileWidth,
								tileHeight / 4
							});

						break;

					default:
						break;
					}

					switch (layerId)
					{
					case 1:
						ecs_set(world, Entityies[nextEntityIndexCount], WorldTile, { tileId });
						break;
					case 4:
						ecs_set(world, Entityies[nextEntityIndexCount], NatureObject, { tileId });
						break;
					default:
						break;
					}

					ecs_set(world, Entityies[nextEntityIndexCount], WorldPosition, { tileX, tileY });
					ecs_set(world, Entityies[nextEntityIndexCount], TextureLocation, { tileId, tileTextureX, tileTextureY });

					nextEntityIndexCount++;
				}
			}

		}

		// Load objects
		// Player etc
		for (int i = 0; i < maxWorldObjects; i++)
		{
			WorldObject* obj = &Objects[i];

			if (strcmp("player_spawn", obj->Name) == 0)
			{
				Entityies[nextEntityIndexCount] = ecs_new_id(world);
				ecs_set(world, Entityies[nextEntityIndexCount], WorldPosition, { obj->x, obj->y });
				ecs_set(world, Entityies[nextEntityIndexCount], Player, { 0 });
				ecs_set(world, Entityies[nextEntityIndexCount], DynamicBody, { -1, obj->x + tileWidth / 2, obj->y + tileHeight / 2,
					tileWidth, tileHeight });
				ecs_set(world, Entityies[nextEntityIndexCount], Velocity, { 0,0 });
				ecs_set(world, Entityies[nextEntityIndexCount], WorldTile, { 0 });
				teleX = obj->x;
				teleY = obj->y;
				nextEntityIndexCount++;
			}

			/*if (strcmp("col", obj->Name) = 0)
			{
				Entityies[nextEntityIndexCount] = ecs_new_id(world);
				ecs_set(world, Entityies[nextEntityIndexCount], StaticBody, { 0 });
				ecs_set(world, Entityies[nextEntityIndexCount], WorldPosition, { obj->x, obj->y });
				ecs_set(world, Entityies[nextEntityIndexCount], WorldTile, { 0 });
				ecs_set(world, Entityies[nextEntityIndexCount], Shape, { RECTANGLE_SHAPE,obj->width, obj->height });
				nextEntityIndexCount++;
			}*/
		}
	}
}
