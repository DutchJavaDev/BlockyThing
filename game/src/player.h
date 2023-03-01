#pragma once
static void SetPlayerCamera(int halfScreenWidth, int halfScreenHeight)
{
	playerCamera.offset = (Vector2)
	{
		halfScreenWidth,
		halfScreenHeight
	};
	playerCamera.target = (Vector2){ 0,0 };
	playerCamera.zoom = 2.0f;
	playerCamera.rotation = 0;
}

static int InCameraView(int targetX, int targetY, int targetWidth, int targetHeight)
{
	int x = playerCamera.target.x - playerCamera.offset.x / 2;
	int y = playerCamera.target.y - playerCamera.offset.y / 2;
	int width = playerCamera.offset.x;
	int height = playerCamera.offset.y;

	Rectangle cameraBox = (Rectangle){ x, y, width , height };
	Rectangle item = (Rectangle){ targetX, targetY, targetWidth, targetHeight };
	return CheckCollisionRecs(cameraBox, item);
}

static void CameraFollow(WorldPosition* targetPosition, DynamicBody* targetBody)
{
	// Follow the player
	Vector2 targetbody =
	{
		targetPosition->x + targetBody[0].width / 2,
		targetPosition->y + targetBody[0].height / 2
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

	playerCamera.zoom += ((float)GetMouseWheelMove() * 0.55f);

	/*if (playerCamera.zoom > MAX_ZOOM)
		playerCamera.zoom = MAX_ZOOM;
	else if (playerCamera.zoom < 1.0f)
		playerCamera.zoom = 1.0f;*/

	UpdateCamera(&playerCamera);
}

static void HanldePlayerInput(Velocity* velocity, WorldPosition* position)
{
	if (IsKeyReleased(KEY_SPACE))
	{
		position->x = playerSpawn.x;
		position->y = playerSpawn.y;
		velocity->x = 0;
		velocity->y = 0;
	}

	if (velocity->x > 0)
		velocity->x = Clamp(velocity->x - GetFrameTime(), 0, WORLD_SPEED);

	if (velocity->x < 0)
		velocity->x = Clamp(velocity->x + GetFrameTime(), -WORLD_SPEED, 0);

	if (velocity->y > 0)
		velocity->y = Clamp(velocity->y - GetFrameTime(), 0, WORLD_SPEED);

	if (velocity->y < 0)
		velocity->y = Clamp(velocity->y + GetFrameTime(), -WORLD_SPEED, 0);

	float _velocity = GetFrameTime() * WORLD_SPEED;

	if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
	{
		velocity->x -= _velocity;
	}
	if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
	{
		velocity->x += _velocity;
	}
	if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
	{
		velocity->y -= _velocity;
	}
	if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
	{
		velocity->y += _velocity;
	}
}