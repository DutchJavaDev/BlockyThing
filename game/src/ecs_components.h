#pragma once
#define CHAR_LOAD_SIZE 32
#pragma region GameComponenets
typedef struct {
	int id;
}Player;

typedef struct
{
	float x;
	float y;
} Velocity;

typedef struct {
	float x;
	float y;
} WorldPosition;

typedef struct {
	Color color;
}ShapeColor;

typedef struct
{
	int id;
	int centerX;
	int centerY;
	int width;
	int height;
}StaticBody;

typedef struct
{
	int id;
	int centerX;
	int centerY;
	int width;
	int height;
} DynamicBody;


typedef struct
{
	int id;
} NatureObject;

typedef struct
{
	int id;
	int textureId;
	float x;
	float y;
}TextureLocation;
#pragma endregion


typedef struct
{
	char Name[28];
	float x;
	float y;
	int width;
	int height;
} WorldObject;

typedef struct
{
	int id;
	int x;
	int y;
} TexturePosition;

typedef struct
{
	int id;
	int x;
	int y;
} WorldTile;


typedef struct {
	int tileId;
	int x;
	int y;
	int width;
	int height;
}CollisionObject;



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

typedef struct {
	char name[CHAR_LOAD_SIZE];
	int x;
	int y;
}Object;


ECS_COMPONENT_DECLARE(Player);
ECS_COMPONENT_DECLARE(Velocity);
ECS_COMPONENT_DECLARE(WorldPosition);
ECS_COMPONENT_DECLARE(ShapeColor);
ECS_COMPONENT_DECLARE(StaticBody);
ECS_COMPONENT_DECLARE(DynamicBody);
ECS_COMPONENT_DECLARE(NatureObject);
ECS_COMPONENT_DECLARE(TextureLocation);
#define MAX_ENTITY_COUNT 456 * 218
const ecs_entity_t* Entityies[MAX_ENTITY_COUNT];
int nextEntityIndexCount = 0;

static ecs_world_t* createWorld()
{
	ecs_world_t* world = ecs_init();

	ECS_COMPONENT_DEFINE(world, Player);
	ECS_COMPONENT_DEFINE(world, Velocity);
	ECS_COMPONENT_DEFINE(world, WorldPosition);
	ECS_COMPONENT_DEFINE(world, ShapeColor);
	ECS_COMPONENT_DEFINE(world, StaticBody);
	ECS_COMPONENT_DEFINE(world, DynamicBody);
	ECS_COMPONENT_DEFINE(world, NatureObject);
	ECS_COMPONENT_DEFINE(world, TextureLocation);
	memset(&Entityies, 0, MAX_ENTITY_COUNT);

	// For web inspector for entities
	/*ecs_singleton_set(world, EcsRest, { 0 });
	ECS_IMPORT(world, FlecsMonitor);*/

	return world;
}

static void AddStaticBody(ecs_world_t* auril_world, int tileId, int centerX, int centerY, int width, int height)
{
	ecs_entity_t* static_body = ecs_new_id(auril_world);
	ecs_set(auril_world, static_body, WorldPosition, { centerX, centerY });
	ecs_set(auril_world, static_body, StaticBody, { tileId, centerX + width / 2, centerY + height / 2,width,height });
}