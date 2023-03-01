/*
 Place to store/get data about the tiles for auril
*/
static void LoadTileTexturePositions(char* fileName);
static TexturePosition* GetTileTexturePosition(int id);
static int IsTileInView(WorldTile* tile);

// 2d array of the map
size_t  rows;
size_t  columns;
WorldTile** tileArray;

size_t texturePositionSize;
size_t texturePositionIndex;
TexturePosition* texturePositions;

Texture2D worldTileSet;

// allocate memory for array for tiles
static void InitTileManager(int width, int height, char* tileTextureFile)
{
	rows = width;
	columns = height;
	tileArray = malloc(sizeof(WorldTile) * rows);


	for (size_t rowIndex = 0; rowIndex < rows; rowIndex++)
	{
		tileArray[rowIndex] = malloc(sizeof(WorldTile) * columns);
	}

	if (tileArray == NULL)
	{
		printf("Failed to allocate memory for tiles, welp");
		exit(-6541);
	}

	LoadTileTexturePositions(tileTextureFile);

	worldTileSet = LoadTexture("world_tiles.png");
}

//DEALlOCATE
static void FreeTileManager(void)
{
	free(tileArray);
	free(texturePositions);
	UnloadTexture(worldTileSet);
}

// method to add tile 
static void AddTile(int id, int x, int y)
{
	WorldTile* tile = &tileArray[x][y];
	tile->id = id;
	tile->x = x * tileWidth;
	tile->y = y * tileHeight;
}

// method to render tiles
static void RenderTiles()
{
	for (size_t rowIndex = 0; rowIndex < rows; rowIndex++)
	{
		// Init tiles
		for (size_t columnIndex = 0; columnIndex < columns; columnIndex++)
		{
			WorldTile* tile = &tileArray[rowIndex][columnIndex];

			// Only tiles that are inside the camera view
			if (IsTileInView(tile))
			{
				TexturePosition* texture = GetTileTexturePosition(tile->id);

				Rectangle source = (Rectangle){ texture->x, texture->y, tileWidth, tileHeight };

				Vector2 position = (Vector2){ tile->x, tile->y };

				DrawTextureRec(worldTileSet, source, position, RAYWHITE);
			}
		}
	}

	// Highlight tile player is standing on
	// bugg because camera is clamped to not be lower than 0 or higher than world width height
	/*int x = playerCamera.target.x / tileWidth;
	int y = playerCamera.target.y / tileHeight;
	WorldTile* tile = &tileArray[x][y];

	if (tile != NULL)
	{
		DrawRectangleLines(tile->x, tile->y, tileWidth, tileHeight, GREEN);
	}*/
}

static TexturePosition* GetTileTexturePosition(int id)
{
	return &texturePositions[id];
}

static int IsTileInView(WorldTile* tile)
{
	return InCameraView(tile->x, tile->y, tileWidth, tileHeight);
}

static void LoadTileTexturePositions(char* fileName)
{
	FILE* file = fopen(fileName, "rb");

	if (file == NULL)
	{
		printf("Failed to open file %s", fileName);
		exit(-5645);
	}

	// Allocate memory for one to start off
	texturePositionSize = sizeof(TexturePosition) * 1;
	texturePositionIndex = 0;
	texturePositions = (TexturePosition*)malloc(texturePositionSize);

	TexturePosition pos;

	while (fread(&pos, sizeof(TexturePosition), 1, file))
	{
		if (texturePositionIndex == 0)
		{
			texturePositions[texturePositionIndex] = pos;
			++texturePositionIndex;
		}
		else
		{
			// This is DANGER, reallocate can fail?
			texturePositionSize += sizeof(TexturePosition);
			texturePositions = realloc(texturePositions, texturePositionSize);
			if (texturePositions == NULL)
			{
				printf("Failed to reallocate memory for texturepositions");
				exit(-354354);
			}
			texturePositions[texturePositionIndex] = pos;
			++texturePositionIndex;
		}
	}
}