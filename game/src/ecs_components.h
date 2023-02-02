#pragma once
#include "raylib.h"

#define RECTANGLE_SHAPE 0
#define CIRCLE_SHAPE 1
#define TRIANGLE_SHAPE 2
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
	int shapeId;
	float baseWidth;
	float baseHeight;
}Shape;

typedef struct {
	Color color;
}ShapeColor;

typedef struct
{
	int id;
}StaticBody;

typedef struct
{
	int id;
} DynamicBody;

typedef struct {
	int tileType;
}WorldTile;

ECS_COMPONENT_DECLARE(WorldTile);
ECS_COMPONENT_DECLARE(Player);
ECS_COMPONENT_DECLARE(Velocity);
ECS_COMPONENT_DECLARE(WorldPosition);
ECS_COMPONENT_DECLARE(Shape);
ECS_COMPONENT_DECLARE(ShapeColor);
ECS_COMPONENT_DECLARE(StaticBody);
ECS_COMPONENT_DECLARE(DynamicBody);

#define MAX_ENTITY_COUNT 456
const ecs_entity_t* Entityies[MAX_ENTITY_COUNT];
int nextEntityIndexCount = 0;

static ecs_world_t* initBlockyThingWorld()
{
	ecs_world_t* world = ecs_init();

	ECS_COMPONENT_DEFINE(world, WorldTile);
	ECS_COMPONENT_DEFINE(world, Player);
	ECS_COMPONENT_DEFINE(world, Velocity);
	ECS_COMPONENT_DEFINE(world, WorldPosition);
	ECS_COMPONENT_DEFINE(world, Shape);
	ECS_COMPONENT_DEFINE(world, ShapeColor);
	ECS_COMPONENT_DEFINE(world, StaticBody);
	ECS_COMPONENT_DEFINE(world, DynamicBody);

	memset(&Entityies, 0, MAX_ENTITY_COUNT);

	// For web inspector for entities
	/*ecs_singleton_set(world, EcsRest, { 0 });
	ECS_IMPORT(world, FlecsMonitor);*/

	return world;
}
