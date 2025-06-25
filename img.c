/*
 * TODO: use *.png to test this - code should auto detect the color that's new - 
 * use https://pixlr.com/editor/ to create more test files
*/
/*compile with -lIL*/
#include <IL/il.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * ideas:
 *  hashmap with x% fudge factor - each bucket contains fuzzy rgb combos - (0-10,0-10,0-10) - etc.
*/

/* we can regnerate the pixmap over the same frames with differing fudge factors until we find a new RGB color
 * starting with more granularity - start with very small fudge factor - this should also be a float perhaps
 */
struct pixmap{
    uint8_t fudge_factor;

    // there will be (255 / fudge_factor) buckets
    /*struct pm_entry** buckets;*/
    struct pm_bucket** buckets;

};

struct pm_bucket{
    struct pm_entry* pixels;
    int n_pixels;
};

/*should i record the RGB values of each bucket? i think so. can also just separate */
struct pm_entry{
    struct pm_entry* next;
    uint8_t rgb[3];
};

void init_pixmap(struct pixmap* pm, uint8_t fudge_factor) {
    pm->fudge_factor = fudge_factor;
    pm->buckets = calloc((255 / fudge_factor), sizeof(struct pm_bucket*));
}

void insert_pixmap(struct pixmap* pm, uint8_t r, uint8_t g, uint8_t b) {
    (void)pm;
    (void)r;
    (void)g;
    (void)b;
    /*int bucket_idx = 0;*/
    /*pm->buckets[*/
}


/*
 * analyze all pixels, 
 * count number of pixels of each color - make a map!
*/

int main(int argc, char* argv[]) {
    ILuint img_id;
    int width;
    int height;
    uint8_t* data;

    if (argc == 1) {
        return EXIT_FAILURE;
    }

    ilInit();
	ilGenImages(1, &img_id);
	ilBindImage(img_id);
	if (!ilLoadImage(argv[1])) {
        puts("failed to open image");
		return EXIT_FAILURE;
	}

    width = ilGetInteger(IL_IMAGE_WIDTH);
    height = ilGetInteger(IL_IMAGE_HEIGHT);
    data = malloc(width * height * 3);

    printf("image of size: %i X %i\n", width, height);
    printf("copy returned: %i\n", ilCopyPixels(0, 0, 0, width, height, 1, IL_RGB, IL_UNSIGNED_BYTE, data));

    /*
     * i can make a map of all RGB combos - no should define a hashmap implementation
     */

    // this works on a row / WIDTH first basis - [0] -> [4] is first row
    // [4] - [8] is second row
    // LEFT TO RIGHT
    for (int i = 0; i < width * height; ++i) {
        /*printf("data[%i]: [%i,%i,%i]\n", i, data[(i * 3)], data[1 + (i * 3)], data[2 + (i * 3)]);*/
    }
	ilDeleteImages(1, &img_id);

}

