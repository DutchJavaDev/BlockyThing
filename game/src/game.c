#include "archi.h"
//----------------------------------------------------------------------------------
// Shared Variables Definition (global)
//----------------------------------------------------------------------------------
#define W 1200
#define STATIC_BODY_COUNT 25
#define BLOCK_SIZE 50
const float hitBoxMultiplier = 1.0f;
const int screenWidth = W;
const int screenHeight = W / 16 * 9;
Camera2D playerCamera;
ecs_world_t* _world;
Texture2D img;
Texture2D player;
// Functions

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
static const float MAX_ZOOM = 7.0f;
static const int WORLD_SPEED = 10;
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
	//---------------------------------------------------------LoadFileText
	SetRandomSeed((int)time(NULL)); // Let there be randomness
	// FLECS init
	ecs_world_t* world = initBlockyThingWorld();

	// SYSTEMS
	InitSystems(world);

	_world = world;
	FILE* worldFile = fopen("resources/map0..cbin", "r");
	if (worldFile == NULL)
	{
		exit(-1);
	}
	LoadWorld(world, worldFile);
	fclose(worldFile);

	playerCamera.offset = (Vector2)
	{
		screenWidth / 2,
		screenHeight / 2
	};
	playerCamera.target = (Vector2){ 0,0 };
	playerCamera.zoom = 2.0f;
	playerCamera.rotation = 0;

	// raylib init
	InitWindow(screenWidth, screenHeight, "BlockyThing");

	img = LoadTexture("resources/world_tiles.png");
	player = LoadTexture("resources/player.png");
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
	CloseWindow();          // Close window and OpenGL context
	//--------------------------------------------------------------------------------------
	UnloadTexture(img);
	UnloadTexture(player);
	return ecs_fini(world);
}


void UpdateDrawFrame(ecs_world_t* world)
{
	ecs_progress(world, 0);
}

void CheckUserInput(ecs_iter_t* it)
{

	Velocity* velocity = ecs_field(it, Velocity, 2);
	WorldPosition* position = ecs_field(it, WorldPosition, 3);

	if (IsKeyReleased(KEY_SPACE))
	{
		position->x = teleX;
		position->y = teleY;
		velocity->xVelocity = 0;
		velocity->yVelocity = 0;
	}


	if (velocity->xVelocity > 0)
		velocity->xVelocity = Clamp(velocity->xVelocity - GetFrameTime(), 0, WORLD_SPEED);

	if (velocity->xVelocity < 0)
		velocity->xVelocity = Clamp(velocity->xVelocity + GetFrameTime(), -WORLD_SPEED, 0);

	if (velocity->yVelocity > 0)
		velocity->yVelocity = Clamp(velocity->yVelocity - GetFrameTime(), 0, WORLD_SPEED);

	if (velocity->yVelocity < 0)
		velocity->yVelocity = Clamp(velocity->yVelocity + GetFrameTime(), -WORLD_SPEED, 0);

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
	WorldPosition* positionArray = ecs_field(it, WorldPosition, 2);
	for (int i = 0; i < it->count; i++)
	{
		Velocity* velocity = &velocityArray[i];
		WorldPosition* position = &positionArray[i];
		position->x += velocity->xVelocity;
		position->y += velocity->yVelocity;
	}
}
void CollisionDetection(ecs_iter_t* it)
{
	WorldPosition* dynamicPositionArray = ecs_field(it, WorldPosition, 1);
	Velocity* dynamicVelocityArray = ecs_field(it, Velocity, 2);
	DynamicBody* dynamicBodyArray = ecs_field(it, DynamicBody, 3);
	// free!
	ecs_filter_t* _static_bodies_filter = ecs_filter(_world, {
   .terms = {
	   { ecs_id(WorldPosition) }, { ecs_id(StaticBody) }
   }
		});

	// Get all static bodies
	ecs_iter_t static_bodies_iterator =
		ecs_filter_iter(_world, _static_bodies_filter);

	while (ecs_filter_next(&static_bodies_iterator))
	{
		WorldPosition* staticPositionArray = ecs_field(&static_bodies_iterator, WorldPosition, 1);
		StaticBody* staticBodyArray = ecs_field(&static_bodies_iterator, StaticBody, 2);

		for (size_t ii = 0; ii < static_bodies_iterator.count; ii++)
		{
			WorldPosition* staticPosition = &staticPositionArray[ii];
			StaticBody* staticBody = &staticBodyArray[ii];
			// create a hit box around self
			// +50% of self

			// This can be done on entity init with custom component instead of here :)
			float parentCenterX = staticBody->centerX;
			float parentCenterY = staticBody->centerY;
			float parentWidth = staticBody->baseWidth;
			float parentHeight = staticBody->baseHeight;

			float hitBoxWidth = parentWidth * hitBoxMultiplier;
			float hitBoxHeight = parentHeight * hitBoxMultiplier;
			float hitBoxX = parentCenterX - hitBoxWidth / 2;
			float hitBoxY = parentCenterY - hitBoxHeight / 2;

			Rectangle staticHitbox = { hitBoxX, hitBoxY, hitBoxWidth, hitBoxHeight };
			Rectangle parentHitbox = { staticPosition->x,staticPosition->y,parentWidth,parentHeight };

			for (size_t i = 0; i < it->count; i++)
			{
				WorldPosition* dynamicPosition = &dynamicPositionArray[i];
				Velocity* velocity = &dynamicVelocityArray[i];
				DynamicBody dynamicBody = dynamicBodyArray[i];

				Rectangle dynamicHitbox = (Rectangle){ dynamicPosition->x, dynamicPosition->y, dynamicBody.baseWidth, dynamicBody.baseHeight };

				if (CheckCollisionRecs(staticHitbox, dynamicHitbox))
				{
					// start checking if colliding with parent
					if (CheckCollisionRecs(dynamicHitbox, parentHitbox))
					{
						// center point
						float dynamicCenterX = dynamicHitbox.x + dynamicHitbox.width / 2;
						float dynamicCenterY = dynamicHitbox.y + dynamicHitbox.height / 2;

						// difference between center points
						float deltaX = dynamicCenterX - parentCenterX;
						float deltaY = dynamicCenterY - parentCenterY;

						// Vertical collision (up or down)
						if (abs(deltaX) < abs(deltaY))
						{

							// bottom collision
							if (deltaY < 0)
							{
								// snap to position
								dynamicPosition->y = staticHitbox.y - dynamicBody.baseHeight;
							}

							// top collision
							if (deltaY > 0)
							{
								// snap to position
								dynamicPosition->y = staticHitbox.y + dynamicBody.baseHeight;
							}

							velocity->yVelocity = 0;

						}

						if (abs(deltaX) > abs(deltaY))
						{
							// right side collision
							if (deltaX < 0)
							{
								// snap to position
								dynamicPosition->x = staticHitbox.x - dynamicBody.baseWidth;
							}

							// left side collision
							if (deltaX > 0)
							{
								// snap to position
								dynamicPosition->x = staticHitbox.x + dynamicBody.baseWidth;
							}

							// will happen next frame 
							velocity->xVelocity = 0;
						}
					}
				}
			}
		}
	}
	ecs_filter_fini(_static_bodies_filter);
}

void UpdatePlayerCamera(ecs_iter_t* it)
{
	WorldPosition* targetPosition = ecs_field(it, WorldPosition, 1);
	DynamicBody* targetBody = ecs_field(it, DynamicBody, 2);

	// Follow the player
	Vector2 targetbody =
	{
		targetPosition->x + targetBody[0].baseWidth / 2,
		targetPosition->y + targetBody[0].baseWidth / 2
	};

	playerCamera.target = targetbody;

	// Clamp the playerCamera x body when its lower than 0
	if (playerCamera.target.x - playerCamera.offset.x / 2 < 0)
	{
		playerCamera.target.x = playerCamera.offset.x / 2;
	}
	// Clamp the playerCamera x body when its higher than WORLD_WIDTH
	if (playerCamera.target.x + playerCamera.offset.x / 2 > worldWidth)
	{
		playerCamera.target.x = worldWidth - playerCamera.offset.x / 2;
	}
	// Clamp the playerCamera y body when its lower than 0
	if (playerCamera.target.y - playerCamera.offset.y / 2 < 0)
	{
		playerCamera.target.y = playerCamera.offset.y / 2;
	}
	// Clamp the playerCamera y body when its higher than WORLD_HEIGHT
	if (playerCamera.target.y + playerCamera.offset.y / 2 > worldHeight)
	{
		playerCamera.target.y = worldHeight - playerCamera.offset.y / 2;
	}

	playerCamera.zoom += ((float)GetMouseWheelMove() * 0.75f);

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
	DrawRectangle(0, 0, worldWidth, worldHeight, DARKGRAY);
	DrawRectangleLines(0, 0, worldWidth, worldHeight, RED);
	EndMode2D(playerCamera);
}

void RenderTiles(ecs_iter_t* it)
{
	WorldPosition* posArray = ecs_field(it, WorldPosition, 1);
	TextureLocation* textureLocationArray = ecs_field(it, TextureLocation, 2);

	BeginMode2D(playerCamera);
	for (int i = 0; i < it->count; i++)
	{
		WorldPosition position = posArray[i];
		TextureLocation texture = textureLocationArray[i];
		Rectangle tileRec = { texture.textureX, texture.textureY, tileWidth, tileHeight };
		DrawTextureRec(img, tileRec, (Vector2) { position.x, position.y }, RAYWHITE);
	}
	EndMode2D(playerCamera);
}

void RenderNatureObjects(ecs_iter_t* it)
{
	WorldPosition* posArray = ecs_field(it, WorldPosition, 1);
	NatureObject* natureObjArray = ecs_field(it, NatureObject, 2);
	TextureLocation* textureLocationArray = ecs_field(it, TextureLocation, 3);
	BeginMode2D(playerCamera);
	for (int i = 0; i < it->count; i++)
	{
		WorldPosition position = posArray[i];
		NatureObject obj = natureObjArray[i];
		TextureLocation texture = textureLocationArray[i];
		Rectangle tileRec = { texture.textureX, texture.textureY, tileWidth, tileHeight };
		DrawTextureRec(img, tileRec, (Vector2) { position.x, position.y }, RAYWHITE);
	}
	EndMode2D(playerCamera);
}

void RenderPlayer(ecs_iter_t* it)
{
	BeginMode2D(playerCamera);
	WorldPosition* posArray = ecs_field(it, WorldPosition, 1);
	WorldPosition position = posArray[0];
	DrawTexture(player, position.x, position.y, WHITE);
	EndMode2D(playerCamera);
}

//void RenderPosition(ecs_iter_t* it)
//{
//	WorldPosition* positionArray = ecs_field(it, WorldPosition, 1);
//	Shape* shapeArray = ecs_field(it, Shape, 2);
//
//	BeginMode2D(playerCamera);
//	for (size_t i = 0; i < it->count; i++)
//	{
//		WorldPosition position = positionArray[i];
//		Shape shape = shapeArray[i];
//
//		char* positionText = TextFormat("x: %i , y: %i", (int)position.x, (int)position.y);
//		DrawText(positionText, position.x + shape.baseWidth / 2, position.y - 10, 20, RED);
//	}
//	EndMode2D(playerCamera);
//}

void RenderStaticHitBox(ecs_iter_t* it)
{
	WorldPosition* positionArray = ecs_field(it, WorldPosition, 1);
	StaticBody* staticBodyArray = ecs_field(it, StaticBody, 2);

	BeginMode2D(playerCamera);
	for (size_t i = 0; i < it->count; i++)
	{
		WorldPosition position = positionArray[i];
		StaticBody body = staticBodyArray[i];

		float parentCenterX = body.centerX;
		float parentCenterY = body.centerY;

		float hitBoxWidth = body.baseWidth * hitBoxMultiplier;
		float hitBoxHeight = body.baseHeight * hitBoxMultiplier;
		float hitBoxX = parentCenterX - hitBoxWidth / 2;
		float hitBoxY = parentCenterY - hitBoxHeight / 2;

		Rectangle hitbox = { hitBoxX, hitBoxY, hitBoxWidth, hitBoxHeight };

		DrawRectangleRec(hitbox, (Color) { 255, 0, 0, 255 * 0.25 });
	}
	EndMode2D(playerCamera);
}

void RenderDynamicHitBox(ecs_iter_t* it)
{
	WorldPosition* positionArray = ecs_field(it, WorldPosition, 1);
	DynamicBody* dynamicBodyArray = ecs_field(it, DynamicBody, 2);

	BeginMode2D(playerCamera);
	for (size_t i = 0; i < it->count; i++)
	{
		WorldPosition position = positionArray[i];
		DynamicBody body = dynamicBodyArray[i];

		float parentCenterX = position.x + (body.baseWidth / 2.0f);
		float parentCenterY = position.y + (body.baseHeight / 2.0f);

		float hitBoxWidth = body.baseWidth * hitBoxMultiplier;
		float hitBoxHeight = body.baseHeight * hitBoxMultiplier;
		float hitBoxX = parentCenterX - hitBoxWidth / 2;
		float hitBoxY = parentCenterY - hitBoxHeight / 2;

		Rectangle hitbox = { hitBoxX, hitBoxY, hitBoxWidth, hitBoxHeight };

		DrawRectangleRec(hitbox, (Color) { 255, 0, 0, 255 * 0.25 });
	}
	EndMode2D(playerCamera);
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
