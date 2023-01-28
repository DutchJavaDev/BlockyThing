#include "archi.h"

//----------------------------------------------------------------------------------
// Shared Variables Definition (global)
//----------------------------------------------------------------------------------
#define W 1200
#define STATIC_BODY_COUNT 25
#define BLOCK_SIZE 50
const int screenWidth = W;
const int screenHeight = W / 16 * 9;
Camera2D playerCamera;
ecs_query_t* _static_bodies_query;
ecs_world_t* _world;
//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
static const float MAX_ZOOM = 7.0f;
static const int WORLD_SPEED = 5;
static const int WORLD_WIDTH = W / 2;
static const int WORLD_HEIGHT = W / 2;
static ecs_entity_t* STATIC_BODIES[STATIC_BODY_COUNT];
//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
void UpdateDrawFrame(ecs_world_t* world);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
	// Initialization
	//---------------------------------------------------------
	playerCamera.offset = (Vector2)
	{
		screenWidth / 2,
		screenHeight / 2
	};
	playerCamera.target = (Vector2){ 0,0 };
	playerCamera.zoom = 2.0f;
	playerCamera.rotation = 0;

	InitWindow(screenWidth, screenHeight, "BlockyThing");

	// FLECS init
	ecs_world_t* world = ecs_init();
	_world = world;

	ECS_COMPONENT(world, Player);
	ECS_COMPONENT(world, Velocity);
	ECS_COMPONENT(world, Position);
	ECS_COMPONENT(world, Shape);
	ECS_COMPONENT(world, ShapeColor);
	ECS_COMPONENT(world, StaticBody);
	ECS_COMPONENT(world, DynamicBody);

	// testing
	ecs_entity_t* object = ecs_new_id(world);
	ecs_set(world, object, Player, { 0 });
	ecs_set(world, object, Velocity, { 0,0 });
	ecs_set(world, object, Position, { 150,150 });
	ecs_set(world, object, Shape, { RECTANGLE_SHAPE,BLOCK_SIZE,BLOCK_SIZE });
	ecs_set(world, object, ShapeColor, { RED });
	ecs_set(world, object, DynamicBody, { 0 });

	for (int i = 0; i < STATIC_BODY_COUNT; i++)
	{
		STATIC_BODIES[i] = ecs_new_id(world);

		float x = GetRandomValue(BLOCK_SIZE, WORLD_WIDTH - BLOCK_SIZE);
		float y = GetRandomValue(BLOCK_SIZE, WORLD_HEIGHT - BLOCK_SIZE);

		Color c;
		c.r = GetRandomValue(0, 255);
		c.g = GetRandomValue(0, 255);
		c.b = GetRandomValue(0, 255);
		c.a = 255;

		int shape = RECTANGLE_SHAPE;

		ecs_set(world, STATIC_BODIES[i], Position, { x,y });
		ecs_set(world, STATIC_BODIES[i], Shape, { shape,BLOCK_SIZE,BLOCK_SIZE });
		ecs_set(world, STATIC_BODIES[i], ShapeColor, { c });
		ecs_set(world, STATIC_BODIES[i], StaticBody, { 0 });
	}

	_static_bodies_query = ecs_query(world, {
		.filter.terms =
		{
			{
				.id = ecs_id(Position)
			},
			{
				.id = ecs_id(Shape)
			},
			{
				.id = ecs_id(StaticBody)
			}
		} });

	// SYSTEMS
	ECS_SYSTEM(world, CheckUserInput, EcsPreUpdate, Player, Velocity, DynamicBody);
	ECS_SYSTEM(world, ApplyVelocity, EcsOnUpdate, Velocity, Position, DynamicBody);
	ECS_SYSTEM(world, CollisionDetection, EcsOnValidate, Position, Shape, DynamicBody);
	ECS_SYSTEM(world, UpdatePlayerCamera, EcsPostUpdate, Position, Shape, Player);
	ECS_SYSTEM(world, BeginRendering, EcsPreStore);
	ECS_SYSTEM(world, RenderWorld, EcsOnStore, Position, Shape, ShapeColor);
	ECS_SYSTEM(world, RenderPosition, EcsOnStore, Position, Shape);
	ECS_SYSTEM(world, EndRendering, EcsOnStore);
#if defined(PLATFORM_WEB)
	emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
	SetTargetFPS(60);       // Set our game to run at 60 frames-per-second
	//--------------------------------------------------------------------------------------

	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		UpdateDrawFrame(world);
	}
#endif
	ecs_fini(world);

	CloseWindow();          // Close window and OpenGL context
	//--------------------------------------------------------------------------------------
	return 0;
}


void UpdateDrawFrame(ecs_world_t* world)
{
	ecs_progress(world, GetFrameTime());
}

void CheckUserInput(ecs_iter_t* it)
{
	Velocity* velocity = ecs_field(it, Velocity, 2);

	if (velocity->xVelocity > 0)
		velocity->xVelocity = velocity->xVelocity - GetFrameTime();

	if (velocity->xVelocity < 0)
		velocity->xVelocity = velocity->xVelocity + GetFrameTime();

	if (velocity->yVelocity > 0)
		velocity->yVelocity = velocity->yVelocity - GetFrameTime();

	if (velocity->yVelocity < 0)
		velocity->yVelocity = velocity->yVelocity + GetFrameTime();

	float _velocity = GetFrameTime() * WORLD_SPEED;

	if (IsKeyDown(KEY_LEFT))
	{
		velocity->xVelocity -= _velocity;
	}
	if (IsKeyDown(KEY_RIGHT))
	{
		velocity->xVelocity += _velocity;
	}
	if (IsKeyDown(KEY_UP))
	{
		velocity->yVelocity -= _velocity;
	}
	if (IsKeyDown(KEY_DOWN))
	{
		velocity->yVelocity += _velocity;
	}
}

void ApplyVelocity(ecs_iter_t* it)
{
	Velocity* velocityArray = ecs_field(it, Velocity, 1);
	Position* positionArray = ecs_field(it, Position, 2);
	for (int i = 0; i < it->count; i++)
	{
		Velocity* velocity = &velocityArray[i];
		Position* position = &positionArray[i];
		position->x += velocity->xVelocity;
		position->y += velocity->yVelocity;
	}
}
void CollisionDetection(ecs_iter_t* it)
{
	Position* dynamicPositionArray = ecs_field(it, Position, 1);
	Shape* dynamicShapeArray = ecs_field(it, Shape, 2);

	// Get all static bodies
	ecs_iter_t static_bodies_iterator =
		ecs_query_iter(_world, _static_bodies_query);

	for (size_t i = 0; i < it->count; i++)
	{
		Position dynamicPosition = dynamicPositionArray[i];
		Shape dynamicShape = dynamicShapeArray[i];

		while (ecs_query_next(&static_bodies_iterator))
		{
			Position* staticPositionArray = ecs_field(&static_bodies_iterator, Position, 1);
			Shape* staticShapeArray = ecs_field(&static_bodies_iterator, Shape, 2);

			for (size_t ii = 0; ii < static_bodies_iterator.count; ii++)
			{

			}
		}
	}
}

void UpdatePlayerCamera(ecs_iter_t* it)
{
	Position* targetPosition = ecs_field(it, Position, 1);
	Shape* targetShape = ecs_field(it, Shape, 2);

	// Follow the player
	Vector2 targetbody =
	{
		targetPosition->x + targetShape->baseWidth / 2,
		targetPosition->y + targetShape->baseHeight / 2
	};

	playerCamera.target = targetbody;

	// Clamp the playerCamera x body when its lower than 0
	if (playerCamera.target.x - playerCamera.offset.x / 2 < 0)
	{
		playerCamera.target.x = playerCamera.offset.x / 2;
	}
	// Clamp the playerCamera x body when its higher than WORLD_WIDTH
	if (playerCamera.target.x + playerCamera.offset.x / 2 > WORLD_WIDTH)
	{
		playerCamera.target.x = WORLD_WIDTH - playerCamera.offset.x / 2;
	}
	// Clamp the playerCamera y body when its lower than 0
	if (playerCamera.target.y - playerCamera.offset.y / 2 < 0)
	{
		playerCamera.target.y = playerCamera.offset.y / 2;
	}
	// Clamp the playerCamera y body when its higher than WORLD_HEIGHT
	if (playerCamera.target.y + playerCamera.offset.y / 2 > WORLD_HEIGHT)
	{
		playerCamera.target.y = WORLD_HEIGHT - playerCamera.offset.y / 2;
	}

	//playerCamera.zoom += ((float)GetMouseWheelMove() * 0.75f);

	/*if (playerCamera.zoom > MAX_ZOOM)
		playerCamera.zoom = MAX_ZOOM;
	else if (playerCamera.zoom < 1.0f)
		playerCamera.zoom = 1.0f;*/

	UpdateCamera(&playerCamera);
}

void BeginRendering(ecs_iter_t* it)
{
	BeginDrawing();
	ClearBackground(BLACK);
	BeginMode2D(playerCamera);
	DrawRectangle(0, 0, WORLD_WIDTH, WORLD_HEIGHT, GREEN);
	DrawRectangleLines(0, 0, WORLD_WIDTH, WORLD_HEIGHT, PINK);
	EndMode2D(playerCamera);
}

void RenderWorld(ecs_iter_t* it)
{
	Position* posArray = ecs_field(it, Position, 1);
	Shape* shapeArray = ecs_field(it, Shape, 2);
	ShapeColor* shapeColorArray = ecs_field(it, ShapeColor, 3);

	BeginMode2D(playerCamera);
	for (int i = 0; i < it->count; i++)
	{
		Position position = posArray[i];
		Shape shape = shapeArray[i];
		Color color = (shapeColorArray[i]).color;

		if (shape.shapeId == RECTANGLE_SHAPE)
		{
			Rectangle rectanlge =
			{
				position.x,
				position.y,
				shape.baseWidth,
				shape.baseHeight
			};
			DrawRectangleRec(rectanlge, color);
			DrawRectangleLinesEx(rectanlge, 1, YELLOW);
		}
		else if (shape.shapeId == CIRCLE_SHAPE)
		{
			int centerX = position.x + shape.baseWidth / 2;
			int centerY = position.y + shape.baseHeight / 2;
			DrawCircle(centerX, centerY, shape.baseWidth * 0.85f, color);
			DrawCircleLines(centerX, centerY, shape.baseWidth * 0.85f, WHITE);
		}
		else
		{
			Vector2 one =
			{
				position.x + shape.baseWidth,
				position.y + shape.baseHeight
			};
			Vector2 two =
			{
				position.x + shape.baseWidth / 2,
				position.y
			};
			Vector2 three =
			{
				position.x,
				position.y + shape.baseHeight
			};
			DrawTriangle(one, two, three, color);
			DrawTriangleLines(one, two, three, GREEN);
		}
	}
	EndMode2D(playerCamera);
}

void RenderPosition(ecs_iter_t* it)
{
	Position* positionArray = ecs_field(it, Position, 1);
	Shape* shapeArray = ecs_field(it, Shape, 2);

	for (size_t i = 0; i < it->count; i++)
	{
		Position position = positionArray[i];
		Shape shape = shapeArray[i];

		char* positionText = TextFormat("x: %i , y: %i", (int)position.x, (int)position.y);
		BeginMode2D(playerCamera);
		DrawText(positionText, position.x + shape.baseWidth / 2, position.y - 10, 20, RED);
		EndMode2D(playerCamera);
	}
}

void EndRendering(ecs_iter_t* it)
{
	// FPS
	Color color = LIME;                         // Good FPS
	int fps = GetFPS();

	if ((fps < 30) && (fps >= 15)) color = ORANGE;  // Warning FPS
	else if (fps < 15) color = RED;             // Low FPS

	DrawText(TextFormat("%2i FPS", GetFPS()), 10, 10, 50, color);
	EndDrawing();
}
