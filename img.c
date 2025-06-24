/*compile with -lIL*/
#include <IL/il.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * analyze all pixels, 
 * count number of pixels of each color - make a map!
*/

int main(int argc, char* argv[]) {
    ILuint img_id;
	ilInit();
	ilGenImages(1, &img_id);
	ilBindImage(img_id);
	if (!ilLoadImage(argv[1])) {
        puts("failed to open image");
		return EXIT_FAILURE;
	}
    int width = ilGetInteger(IL_IMAGE_WIDTH);
    int height = ilGetInteger(IL_IMAGE_HEIGHT);
    uint8_t* data = malloc(width * height * 3);
    printf("copy returned: %i\n", ilCopyPixels(0, 0, 0, width, height, 1, IL_RGB, IL_UNSIGNED_BYTE, data));
    printf("image of size: %i X %i\n", width, height);

    /*
     * i can make a map of all RGB combos - no should define a hashmap implementation
     */

    // this works on a row / WIDTH first basis - [0] -> [4] is first row
    // [4] - [8] is second row
    // LEFT TO RIGHT
    for (int i = 0; i < width * height; ++i) {
        printf("data[%i]: [%i,%i,%i]\n", i, data[(i * 3)], data[1 + (i * 3)], data[2 + (i * 3)]);
    }
	ilDeleteImages(1, &img_id);

}

