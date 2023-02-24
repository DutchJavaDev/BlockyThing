#pragma once
#define CHAR_LOAD_SIZE 32
static const float MAX_ZOOM = 7.0f;
static const int WORLD_SPEED = 10;

int worldWidth = 10;
int worldHeight = 10;
int tileWidth = 32;
int tileHeight = 32;
int nextObjectIndex = 0;

Camera2D playerCamera;
RenderTexture2D tileMapTexture;
Texture2D worldTileSet;
ecs_world_t* auril_world;
Texture2D player;

#pragma region  Objects
Vector2 playerSpawn = { 0 };
#pragma endregion


typedef struct
{
	int id;
	float xOffset;
	float yOffset;
	int tileWidth;
	int tileHeight;
	char imageFile[CHAR_LOAD_SIZE];
	char tileFile[CHAR_LOAD_SIZE];
	char collisionFile[CHAR_LOAD_SIZE];
} TileSet;

typedef struct
{
	char name[32];
	int width;
	int height;
	int tileWidth;
	int tileHeigt;
	int tileSetCount;
	char worldTileFile[CHAR_LOAD_SIZE];
}WorldFile;

typedef struct
{
	int id;
	int xPosition;
	int yPosition;
} WorldTile;

typedef struct
{
	int id;
	int xPosition;
	int yPosition;
} TexturePosition;

typedef struct {
	int tileId;
	int x;
	int y;
	int width;
	int height;
}CollisionObject;

typedef struct {
	char name[CHAR_LOAD_SIZE];
	int x;
	int y;
}Object;

static WorldFile ReadWorldFile(char* path);
static TexturePosition GetTileTexturePosition(FILE* file, int index);
static void LoadObjects(char* path);
static void LoadTileCollisionData(FILE* collisionFile, WorldTile tile, int tileX, int tileY);

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

			FILE* textTureInfoFile;
			textTureInfoFile = fopen("auril_home.tf.0.tfn.bin", "rb");
			if (textTureInfoFile == NULL)
			{
				printf("Unable to load textTureInfoFile file, exiting");
				exit(-3);
			}

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

			WorldTile tile = { 0 };

			worldTileSet = LoadTexture("world_tiles.png");
			tileMapTexture = LoadRenderTexture(worldWidth, worldHeight);
			SetTextureFilter(tileMapTexture.texture, TEXTURE_FILTER_POINT);
			BeginTextureMode(tileMapTexture);
			ClearBackground(PINK);

			do {

				// needed because yea..
				if (tile.id < 0)
				{
					break;
				}

				int tileX = tile.xPosition * tileWidth;
				int tileY = tile.yPosition * tileHeight;

				// Load texture data
				TexturePosition textureLocation = GetTileTexturePosition(textTureInfoFile, tile.id);

				Rectangle textPos = (Rectangle){ textureLocation.xPosition, textureLocation.yPosition,tileWidth, tileHeight };
				Vector2 textPosV2 = (Vector2){ tileX, tileY };

				// Draw tile
				DrawTextureRec(worldTileSet, textPos, textPosV2, WHITE);

				// Load collision data
				LoadTileCollisionData(collisionFile, tile, tileX, tileY);

				// needed because yea..
				tile.id = -1;
			} while (fread(&tile, sizeof(WorldTile), 1, tilemapFile));

			EndTextureMode();

			fclose(tilemapFile);
			fclose(textTureInfoFile);
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

static TexturePosition GetTileTexturePosition(FILE* file, int index)
{
	if (file == NULL)
	{
		printf("Failed to open file");
		exit(1);
	}

	TexturePosition texturePos = { 0 };

	fseek(file, sizeof(TexturePosition) * index, 0);
	fread(&texturePos, sizeof(TexturePosition), 1, file);

	return texturePos;
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

	do
	{
		if (strcmp("player_spanw", obj.name))
		{
			playerSpawn = (Vector2){ obj.x,obj.y };
		}
	} while (fread(&obj, sizeof(Object), 1, file));

	fclose(file);
}

static void LoadTileCollisionData(FILE* collisionFile, WorldTile tile, int tileX, int tileY)
{
	fseek(collisionFile, 0, 0);
	CollisionObject cob = { 0 };
	do {
		if (cob.tileId == tile.id)
		{
			int x = tileX + cob.x;
			int y = tileY + cob.y;
			int width = cob.width;
			int height = cob.height;

			AddStaticBody(auril_world, tile.id, x, y, width, height);
		}
	} while (fread(&cob, sizeof(CollisionObject), 1, collisionFile));
}