#pragma once
#define maxLineBuffer 32
#define maxWorldObjects 32

static const float hitBoxMultiplier = 1.0f;
static const float MAX_ZOOM = 7.0f;
static const int WORLD_SPEED = 10;

int worldWidth = 0;
int worldHeight = 0;
int tileWidth = 0;
int tileHeight = 0;
int teleX = 0;
int teleY = 0;
int nextObjectIndex = 0;

Camera2D playerCamera;
Texture2D worldTileSet;
Texture2D worldNatureSet;
ecs_world_t* auril_world;
Texture2D player;

WorldObject Objects[maxWorldObjects];

static Texture2D ResolveTexture(int index)
{
	switch (index)
	{
	case 0:
		return worldTileSet;
	case 1:
		return worldNatureSet;

	default:
		return worldTileSet; // :)
	}
}

static int GetTileSize(int index)
{
	switch (index)
	{
	case 0:
		return 32;
	case 1:
		return 16;

	default:
		return 64; // :)
	}
}

static void SetPlayerCamera(int halfScreenWidth, int halfScreenHeight)
{
	playerCamera.offset = (Vector2)
	{
		halfScreenWidth,
		halfScreenHeight
	};
	playerCamera.target = (Vector2){ 0,0 };
	playerCamera.zoom = 2.0f;
	playerCamera.rotation = 0;
}

static void LoadWorldFromFile(ecs_world_t* world)
{
	FILE* worldFile = fopen("resources/map0.cbin", "r");
	if (worldFile == NULL)
	{
		exit(-1);
	}

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
					int yOffset;
					int xOffset;
					int textureY;
					int textureX;
					int textureId;

					int rtn = sscanf(lineBuffer, "%d %d %f %f %d %d %d", &layerId, &tileId, &tileX, &tileY, &textureId, &textureX, &textureY);

					if (rtn == 0)
					{
						break;
					}

					Entityies[nextEntityIndexCount] = ecs_new_id(world);

					tileX = tileX * tileWidth;
					tileY = tileY * tileHeight;

					switch (tileId)
					{
					case 0:
						ecs_set(world, Entityies[nextEntityIndexCount], StaticBody, { tileId, tileX + tileWidth / 2, tileY + tileHeight / 2, tileWidth, tileHeight });
						break;

					case 12:
					case 42:
						ecs_set(world, Entityies[nextEntityIndexCount], StaticBody,
							{
								tileId,
								tileX + (tileWidth * 0.2f) / 2,
								tileY + tileHeight / 2,
								(tileWidth * 0.2f),
								tileHeight
							});
						break;

					case 15:
					case 16:
					case 17:
					case 24:
					case 25:
					case 26:
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
					ecs_set(world, Entityies[nextEntityIndexCount], TextureLocation, { tileId, textureId, textureX, textureY });

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

	fclose(worldFile);
}
