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
} Positin;

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