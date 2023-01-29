#pragma once
#include "flecs.h"
void CheckUserInput(ecs_iter_t* it);
void ApplyVelocity(ecs_iter_t* it);
void CollisionDetection(ecs_iter_t* it);
void UpdatePlayerCamera(ecs_iter_t* it);
void BeginRendering(ecs_iter_t* it);
void RenderWorld(ecs_iter_t* it);
void RenderPosition(ecs_iter_t* it);
void RenderHitBox(ecs_iter_t* it);
void EndRendering(ecs_iter_t* it);