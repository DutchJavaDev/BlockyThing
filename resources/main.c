#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include "stdbool.h"

typedef struct
{
    int tileId;
    int xPosition;
    int yPosition;
    int width;
    int height;
} CollisionBody;

typedef struct
{
    int id;
    int xPosition;
    int yPosition;
} Tile;

typedef struct
{
    int id;
    float xOffset;
    float yOffset;
    int tileWidth;
    int tileHeight;
    char imageFile[32];
    char tileFile[32];
    char collisionFile[32];
} TileSet;

int main(void)
{
    FILE *fp;

    TileSet set;

    fp = fopen("auril_home.tf.0.bin", "rb");

    if (fp == NULL)
    {
        printf("Error opening file.\n");
        return 1;
    }

    while (fread(&set, sizeof(TileSet), 1, fp))
    {
        printf("Id: %i\n", set.id);

        printf("xOffset: %i\n", set.xOffset);

        printf("yOffset: %i\n", set.yOffset);

        printf("tileWidth: %i\n", set.tileWidth);

        printf("tileHeight: %i\n", set.tileHeight);

        printf("image: %s\n", set.imageFile);

        printf("tile: %s\n", set.tileFile);

        printf("collision: %s\n", set.collisionFile);

        Tile tile;

        FILE *tileFile = fopen(set.tileFile, "rb");

        printf("Tile data\n");
        while (fread(&tile, sizeof(Tile), 1, tileFile))
        {
            printf("id: %i\n", tile.id);
            printf("x: %i\n", tile.xPosition);
            printf("y: %i\n", tile.yPosition);
        }
        fclose(tileFile);
        
        FILE *collisionFile = fopen(set.collisionFile, "rb");
        
        CollisionBody collision;

        printf("TileCollision data\n");
        while (fread(&collision, sizeof(CollisionBody), 1, collisionFile))
        {
            printf("tileId: %i\n", collision.tileId);
            printf("width: %i\n", collision.width);
            printf("height: %i\n", collision.height);
            printf("x: %i\n", collision.xPosition);
            printf("y: %i\n", collision.yPosition);
        }
        printf("end %i\n", set.id);
    }

    fclose(fp);

    return 0;
}
