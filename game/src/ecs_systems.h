#pragma once
#include "flecs.h"

void CheckUserInput(ecs_iter_t* it);
void ApplyVelocity(ecs_iter_t* it);
void CollisionDetection(ecs_iter_t* it);
void UpdatePlayerCamera(ecs_iter_t* it);
void BeginRendering(ecs_iter_t* it);
void RenderTiles(ecs_iter_t* it);
void RenderNatureObjects(ecs_iter_t* it);
void RenderPlayer(ecs_iter_t* it);
//void RenderPosition(ecs_iter_t* it);
void RenderStaticHitBox(ecs_iter_t* it);
void RenderDynamicHitBox(ecs_iter_t* it);
void EndRendering(ecs_iter_t* it);


static void InitSystems(ecs_world_t* world)
{
	ECS_SYSTEM(world, CheckUserInput, EcsPreUpdate, Player, Velocity, WorldPosition, DynamicBody);
	ECS_SYSTEM(world, ApplyVelocity, EcsOnUpdate, Velocity, WorldPosition, DynamicBody);
	ECS_SYSTEM(world, CollisionDetection, EcsOnValidate, WorldPosition, Velocity, DynamicBody);
	ECS_SYSTEM(world, UpdatePlayerCamera, EcsPostUpdate, WorldPosition, DynamicBody, Player);
	ECS_SYSTEM(world, BeginRendering, EcsPreStore);
	ECS_SYSTEM(world, RenderTiles, EcsOnStore, WorldPosition, TextureLocation, WorldTile);
	ECS_SYSTEM(world, RenderNatureObjects, EcsOnStore, WorldPosition, NatureObject, TextureLocation);
	ECS_SYSTEM(world, RenderPlayer, EcsOnStore, WorldPosition, Player);
	//ECS_SYSTEM(world, RenderPosition, EcsOnStore, WorldPosition, Shape, DynamicBody);
	ECS_SYSTEM(world, RenderStaticHitBox, EcsOnStore, WorldPosition, StaticBody);
	ECS_SYSTEM(world, RenderDynamicHitBox, EcsOnStore, WorldPosition, DynamicBody);
	ECS_SYSTEM(world, EndRendering, EcsOnStore);
}