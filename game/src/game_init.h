#pragma once
#pragma region MapLayersId
#define WALKALBE_PATH_LAYER 1
#define PLAYER_LAYER 5
#define NATURE_OBJECTS_LAYER 4
#define OBJECTS_LAYER 2
#pragma endregion

#pragma region InternalFunctions
#pragma endregion


#define FILE_LINEBUFFER 32
#define MAX_WORLD_OBJECTS 64

static const float hitBoxMultiplier = 1.0f;
static const float MAX_ZOOM = 7.0f;
static const int WORLD_SPEED = 10;

int worldWidth = 10;
int worldHeight = 10;
int tileWidth = 32;
int tileHeight = 32;
int teleX = 5;
int teleY = 5;
int nextObjectIndex = 0;

Camera2D playerCamera;
RenderTexture2D tileMapTexture;
Texture2D worldTileSet;
ecs_world_t* auril_world;
Texture2D player;

typedef struct
{
	int id;
	float xOffset;
	float yOffset;
	int tileWidth;
	int tileHeight;
	char imageFile[32];
	char tileFile[32];
	char collisionFile[32];
} TileSet;

typedef struct
{
	char name[32];
	int width;
	int height;
	int tileWidth;
	int tileHeigt;
	int tileSetCount;
	char worldTileFile[32];
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

WorldObject Objects[MAX_WORLD_OBJECTS];

static Texture2D ResolveTexture(int index)
{
	switch (index)
	{
	case 0:
		return worldTileSet;

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
	playerCamera.target = (Vector2){ 75,75 };
	playerCamera.zoom = 2.0f;
	playerCamera.rotation = 0;
}

static void LoadWorld(ecs_world_t* ecs_world)
{
	if (ChangeDirectory("resources"))
	{
		char* fileName = "auril_home.wf.bin";

		FILE* worldFile;

		worldFile = fopen(fileName, "rb");

		if (worldFile == NULL)
		{
			printf("Unable to load worldfile, exiting");
			exit(-1);
		}

		WorldFile world = { 0 };

		worldTileSet = LoadTexture("world_tiles.png");

		while (fread(&world, sizeof(WorldFile), 1, worldFile))
		{
			worldWidth = world.width;
			worldHeight = world.height;
			tileWidth = world.tileWidth;
			tileHeight = world.tileHeigt;

			tileMapTexture = LoadRenderTexture(worldWidth * tileWidth, worldHeight * tileHeight);
			SetTextureFilter(tileMapTexture.texture, TEXTURE_FILTER_POINT);
			BeginTextureMode(tileMapTexture);

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

			while (fread(&tile, sizeof(WorldTile), 1, tilemapFile))
			{
				// needed because yea..
				if (tile.id < 0)
				{
					break;
				}

				int tileX = tile.xPosition * tileWidth;
				int tileY = tile.yPosition * tileHeight;


				// Load texture data
				TexturePosition textureLocation = { 0 };
				fseek(textTureInfoFile, sizeof(TexturePosition) * tile.id, 0);
				fread(&textureLocation, sizeof(TexturePosition), 1, textTureInfoFile);

				Rectangle textPos = (Rectangle){ textureLocation.xPosition, textureLocation.yPosition,tileWidth, tileHeight };
				Vector2 textPosV2 = (Vector2){ tileX, tileY };

				DrawTextureRec(worldTileSet, textPos, textPosV2, WHITE);

				// Load collision data
				fseek(collisionFile, 0, 0);
				CollisionObject cob = { 0 };
				while (fread(&cob, sizeof(CollisionObject), 1, collisionFile))
				{
					if (cob.tileId == tile.id)
					{
						DrawRectangle(tileX + cob.x, tileY + cob.y, cob.width, cob.height, RED);
					}
				}

				// needed because yea..
				tile.id = -1;
			}
			fclose(tilemapFile);
			fclose(textTureInfoFile);
			EndTextureMode();
		}

		fclose(worldFile);

		ecs_entity_t* player = ecs_new_id(ecs_world);
		ecs_set(ecs_world, player, Player, { 0 });
		ecs_set(ecs_world, player, Velocity, { 0, 0 });
		ecs_set(ecs_world, player, WorldPosition, { 0,0 });
		ecs_set(ecs_world, player, DynamicBody, { 0,150,150,tileWidth,tileHeight });
	}
}