#pragma once
#include "raylib.h"

#pragma region GameComponenets
typedef struct {
	int id;
}Player;

typedef struct
{
	float xVelocity;
	float yVelocity;
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
	int tileId;
	int centerX;
	int centerY;
	int baseWidth;
	int baseHeight;
}StaticBody;

typedef struct
{
	int tileId;
	int centerX;
	int centerY;
	int baseWidth;
	int baseHeight;
} DynamicBody;


typedef struct
{
	int tileId;
} NatureObject;

typedef struct
{
	int tileId;
	int textureId;
	float textureX;
	float textureY;
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
