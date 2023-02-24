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

typedef struct
{
    char name[32];
    int width;
    int height;
    int tileWidth;
    int tileHeigt;
    int tileSetCount;
    char worldTileFile[32];
} WorldData;

int main(void)
{
    FILE *ffp;
    ffp = fopen("auril_home.tf.0.tfn.bin", "rb");

    if (ffp == NULL)
    {
        printf("Yep ya boi failed ya!");
        exit(-1);
    }

    Tile tile;
    fseek(ffp,sizeof(Tile) * 1,0);
    fread(&tile, sizeof(Tile), 1, ffp);
    printf("tileId %i\n", tile.id);
    printf("x %i\n", tile.xPosition);
    printf("y %i\n", tile.yPosition);

    fclose(ffp);

    // FILE *fp;

    // TileSet set;

    // fp = fopen("auril_home.wf.bin", "rb");

    // if (fp == NULL)
    // {
    //     printf("Error opening file.\n");
    //     return 1;
    // }

    // WorldData data = {0};

    // while (fread(&data,sizeof(WorldData),1,fp))
    // {
    //     printf("%i\n",data.tileWidth);
    //     printf("%i\n",data.tileHeigt);

    //     FILE* map;

    //     map = fopen(data.worldTileFile,"rb");

    //     if(map == NULL)
    //     {
    //         printf("Error opening file.\n");
    //         break;
    //     }

    //     Tile tile;
    //     while (fread(&tile, sizeof(Tile),1,map))
    //     {
    //         printf("tileId %i\n", tile.id);
    //         printf("x %i\n", tile.xPosition);
    //         printf("y %i\n", tile.yPosition);
    //     }

    //     fclose(map);
    //     }

    // fclose(fp);

    return 0;
}
