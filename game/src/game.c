#include "archi.h"
//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
int screenWidth = 0;
int screenHeight = 0;
//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
void UpdateDrawFrame(ecs_world_t* world);
//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
	// Pre Initializations
	//---------------------------------------------------------

	// Even there is no such thing as true randomness, we can come close to it:)
	SetRandomSeed((int)time(NULL));
	// FLECS
	auril_world = createWorld();

	setWorldSystems(auril_world);

	// RAYLIB INIT
	InitWindow(screenWidth, screenHeight, "BlockyThing");

	int currentMonitorId = GetCurrentMonitor();

	// Take 85% of the screen and then apply 16:9 ratio, this is a test might change
	screenWidth = GetMonitorWidth(currentMonitorId) * 0.65f;
	screenHeight = screenWidth / 16 * 9;

	// Set the actual size, slight flicker
	SetWindowSize(screenWidth, screenHeight);

	// Center of the current screen
	int x = GetMonitorWidth(currentMonitorId) / 2 - screenWidth / 2;
	int y = GetMonitorHeight(currentMonitorId) / 2 - screenHeight / 2;

	SetWindowPosition(x, y);

	SetPlayerCamera(screenWidth / 2, screenHeight / 2);

	// Load resources
	// Move to function to?
	LoadWorld(auril_world);
	player = LoadTexture("player.png");

#if defined(PLATFORM_WEB)
	emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
	SetTargetFPS(60);       // Set our game to run at 60 frames-per-second
	//--------------------------------------------------------------------------------------

	// Main game loop
	// Detect window close button or ESC key
	while (!WindowShouldClose())
	{
		UpdateDrawFrame(auril_world);
	}
#endif
	CloseWindow();          // Close window and OpenGL context
	//--------------------------------------------------------------------------------------
	UnloadRenderTexture(tileMapTexture);
	UnloadTexture(worldTileSet);
	UnloadTexture(player);
	ecs_fini(auril_world);
	return 0;
}


void UpdateDrawFrame(ecs_world_t* world)
{
	ecs_progress(world, 0);
}

void Input(ecs_iter_t* it)
{
	Velocity* velocity = ecs_field(it, Velocity, 2);
	WorldPosition* position = ecs_field(it, WorldPosition, 3);
	HanldePlayerInput(velocity, position);
}

void Movement(ecs_iter_t* it)
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
	ecs_filter_t* _static_bodies_filter = ecs_filter(auril_world, {
   .terms = {
	   { ecs_id(WorldPosition) }, { ecs_id(StaticBody) }
   }
		});

	// Get all static bodies
	ecs_iter_t static_bodies_iterator =
		ecs_filter_iter(auril_world, _static_bodies_filter);

	RunCollisionSimulations(it, dynamicPositionArray, dynamicVelocityArray, dynamicBodyArray, static_bodies_iterator);

	ecs_filter_fini(_static_bodies_filter);
}

void PlayerCamera(ecs_iter_t* it)
{
	WorldPosition* targetPosition = ecs_field(it, WorldPosition, 1);
	DynamicBody* targetBody = ecs_field(it, DynamicBody, 2);
	//CameraFollow(targetPosition, targetBody);
}

void BeginRendering(ecs_iter_t* it)
{
	BeginDrawing();
	ClearBackground(BLACK);
	BeginMode2D(playerCamera);
	DrawTextureRec(tileMapTexture.texture, (Rectangle) { 0, 0, (float)tileMapTexture.texture.width, (float)-tileMapTexture.texture.height }, (Vector2) { 0, 0 }, WHITE);
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

		//DrawRectangleRec(hitbox, (Color) { 255, 0, 0, 255 * 0.25 });
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
