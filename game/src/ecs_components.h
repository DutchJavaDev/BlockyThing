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

	ECS_COMPONENT_DEFINE(world, Player);
	ECS_COMPONENT_DEFINE(world, Velocity);
	ECS_COMPONENT_DEFINE(world, WorldPosition);
	ECS_COMPONENT_DEFINE(world, Shape);
	ECS_COMPONENT_DEFINE(world, ShapeColor);
	ECS_COMPONENT_DEFINE(world, StaticBody);
	ECS_COMPONENT_DEFINE(world, DynamicBody);

	return world;
}

static void AddStaticEntityAt(ecs_world_t* world, float x, float y, float width, float height)
{
	if (nextEntityIndexCount > MAX_ENTITY_COUNT)
		return;

	Entityies[nextEntityIndexCount] = ecs_new_id(world);

	Color c;
	c.r = GetRandomValue(0, 255);
	c.g = GetRandomValue(0, 255);
	c.b = GetRandomValue(0, 255);
	c.a = 255;

	int shape = GetRandomValue(0, 2);

	ecs_set(world, Entityies[nextEntityIndexCount], WorldPosition, { x,y });
	ecs_set(world, Entityies[nextEntityIndexCount], StaticBody, { 0 });
	ecs_set(world, Entityies[nextEntityIndexCount], Shape, { shape,width,height });
	ecs_set(world, Entityies[nextEntityIndexCount], ShapeColor, { c });

	nextEntityIndexCount++;
}

static void AddDynamicEntityAt(ecs_world_t* world, float x, float y, float width, float height, bool isPlayer)
{
	if (nextEntityIndexCount > MAX_ENTITY_COUNT)
		return;

	Entityies[nextEntityIndexCount] = ecs_new_id(world);

	Color c;
	c.r = GetRandomValue(0, 255);
	c.g = GetRandomValue(0, 255);
	c.b = GetRandomValue(0, 255);
	c.a = 255;

	int shape = GetRandomValue(0, 2);

	ecs_set(world, Entityies[nextEntityIndexCount], WorldPosition, { x,y });
	ecs_set(world, Entityies[nextEntityIndexCount], DynamicBody, { 0 });
	ecs_set(world, Entityies[nextEntityIndexCount], Shape, { shape,width,height });
	ecs_set(world, Entityies[nextEntityIndexCount], ShapeColor, { c });

	if (isPlayer)
	{
		ecs_set(world, Entityies[nextEntityIndexCount], Player, { 0 });
	}

	nextEntityIndexCount++;
}