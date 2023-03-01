#pragma once
static void InitTileManager(int width, int height);
static void AddTile(int id, int x, int y);
static const float MAX_ZOOM = 7.0f;
static const int WORLD_SPEED = 10;

int worldWidth;
int worldHeight;
int tileWidth;
int tileHeight;

Camera2D playerCamera;
Texture2D worldTileSet;
ecs_world_t* auril_world;
Texture2D player;

#pragma region  Objects
Vector2 playerSpawn = { 0 };
#pragma endregion

static WorldFile ReadWorldFile(char* path);
static void LoadObjects(char* path);
static void LoadTileCollisionData(FILE* collisionFile, WorldTile tile);

static void LoadWorld(ecs_world_t* ecs_world)
{
	if (ChangeDirectory("resources"))
	{
		char* worldFileName = "auril_home.wf.bin";
		char* objectFileName = "auril_home.of.bin";
		char* textureInfoFileName = "auril_home.tf.0.tfn.bin";

		WorldFile world = ReadWorldFile(worldFileName);
		{
			tileWidth = world.tileWidth;
			tileHeight = world.tileHeigt;

			worldWidth = world.width * tileWidth;
			worldHeight = world.height * tileHeight;

			InitTileManager(world.width, world.height, textureInfoFileName);

			FILE* tilemapFile;
			tilemapFile = fopen(world.worldTileFile, "rb");
			if (tilemapFile == NULL)
			{
				printf("Unable to load tilemap file, exiting");
				exit(-3);
			}

			FILE* collisionFile;
			collisionFile = fopen("auril_home.tf.0.tfn.cl.bin", "rb");
			if (collisionFile == NULL)
			{
				printf("Unable to load tilemap file, exiting");
				exit(-3);
			}

			WorldTile tile;
			while (fread(&tile, sizeof(WorldTile), 1, tilemapFile))
			{
				AddTile(tile.id, tile.x, tile.y);

				// Load collision data
				LoadTileCollisionData(collisionFile, tile);
			}

			fclose(tilemapFile);
			fclose(collisionFile);

			// Objects
			LoadObjects(objectFileName);
		}

		// spawn player
		ecs_entity_t* player = ecs_new_id(ecs_world);
		ecs_set(ecs_world, player, Player, { 0 });
		ecs_set(ecs_world, player, Velocity, { 0, 0 });
		ecs_set(ecs_world, player, WorldPosition, { playerSpawn.x,playerSpawn.y });
		ecs_set(ecs_world, player, DynamicBody, { 0,150,150,16,16 });
	}
}

static WorldFile ReadWorldFile(char* path)
{
	FILE* file = fopen(path, "rb");

	if (file == NULL)
	{
		printf("Failed to open file %s", path);
		exit(1);
	}

	WorldFile worldFile = { 0 };

	fread(&worldFile, sizeof(WorldFile), 1, file);
	fclose(file);

	return worldFile;
}

static void LoadObjects(char* path)
{
	FILE* file = fopen(path, "rb");

	if (file == NULL)
	{
		printf("Failed to open file %s", path);
		exit(1);
	}

	Object obj = { 0 };

	while (fread(&obj, sizeof(Object), 1, file))
	{
		if (strcmp("player_spanw", obj.name))
		{
			playerSpawn = (Vector2){ obj.x,obj.y };
		}
	}

	fclose(file);
}

static void LoadTileCollisionData(FILE* collisionFile, WorldTile tile)
{
	fseek(collisionFile, 0, 0);
	CollisionObject cob = { 0 };
	while (fread(&cob, sizeof(CollisionObject), 1, collisionFile))
	{
		if (cob.tileId == tile.id)
		{
			int x = (tile.x * tileWidth) + cob.x;
			int y = (tile.y * tileHeight) + cob.y;
			int width = cob.width;
			int height = cob.height;

			AddStaticBody(auril_world, tile.id, x, y, width, height);
		}
	}
}