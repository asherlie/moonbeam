/*
 * TODO: use *.png to test this - code should auto detect the color that's new - 
 * use https://pixlr.com/editor/ to create more test files
*/
/*compile with -lIL*/
#include <IL/il.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
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
    // number of identical pixels
    int n;
    struct pm_entry* next;
    uint8_t rgb[3];
};

void init_pixmap(struct pixmap* pm, uint8_t fudge_factor) {
    uint32_t n_buckets = 0;
    uint8_t* nb_b = (uint8_t*)&n_buckets;

    pm->fudge_factor = fudge_factor;

    nb_b[0] = 255 / fudge_factor;
    nb_b[1] = 255 / fudge_factor;
    nb_b[2] = 255 / fudge_factor;

    pm->buckets = calloc(n_buckets, sizeof(struct pm_bucket*));
    printf("alloc'd %i buckets\n", n_buckets);
}

/*there will be ~17M / fudge_factor buckets*/
/*higher fudge factor will take up less memory*/
/*TODO: should diskmap be used?*/
uint32_t which_bucket(struct pixmap* pm, uint8_t* rgb) {
    uint32_t idx = 0;
    uint8_t* b_idx = (uint8_t*)&idx;
    // set r in idx to the 0-(255/fudge_factor) bucket
    b_idx[0] = rgb[0] / pm->fudge_factor;
    b_idx[1] = rgb[1] / pm->fudge_factor;
    b_idx[2] = rgb[2] / pm->fudge_factor;
    /*printf("r: %i, setting fudged to %i\n", rgb[0], b_idx[0]);*/

    return idx;
/*
 *     if fudge_factor == 2
 *         0: 0
 *         1: 0
 *         2: 1
 *         3: 1
 *         4: 2
 *         5: 2
 * 
 *         AH! this is the FLOOR of (value / fudge_factor)
 *         i believe i can just use integer division
 *         TODO: confirm this
*/
}

void insert_pm_entry(struct pm_bucket* pm_b, uint8_t* rgb) {
    struct pm_entry* pm_e;

    ++pm_b->n_pixels;
    for (pm_e = pm_b->pixels; pm_e; pm_e = pm_e->next) {
        if (!memcmp(pm_e->rgb, rgb, 3)) {
            ++pm_e->n;
            return;
        }
    }
    pm_e = calloc(1, sizeof(struct pm_entry));
    memcpy(pm_e->rgb, rgb, 3);
    pm_e->n = 1;
}

// rgb is a pointer to r with gb after it
void insert_pixmap(struct pixmap* pm, uint8_t* rgb) {
    uint32_t bucket_idx = which_bucket(pm, rgb);

    if (!pm->buckets[bucket_idx]) {
        pm->buckets[bucket_idx] = calloc(1, sizeof(struct pm_bucket));
    }
    insert_pm_entry(pm->buckets[bucket_idx], rgb);
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

    struct pixmap pm;
    uint8_t rgb[3] = {0, 0, 9};
    uint8_t* rgbp = rgb;
    init_pixmap(&pm, 1);
    insert_pixmap(&pm, rgbp);
    /*printf("which returned %i\n", which_bucket(&pm, rgbp));*/

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

