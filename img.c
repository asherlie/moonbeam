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
    uint32_t n_buckets;

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
    uint8_t* nb_b = (uint8_t*)&pm->n_buckets;

    pm->n_buckets = 0;
    pm->fudge_factor = fudge_factor;

    nb_b[0] = 255 / fudge_factor;
    nb_b[1] = 255 / fudge_factor;
    nb_b[2] = 255 / fudge_factor;

    ++pm->n_buckets;

    pm->buckets = calloc(pm->n_buckets, sizeof(struct pm_bucket*));
    /*printf("alloc'd %i buckets\n", pm->n_buckets);*/
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
    pm_e->next = pm_b->pixels;
    pm_b->pixels = pm_e;
}

// rgb is a pointer to r with gb after it
void insert_pixmap(struct pixmap* pm, uint8_t* rgb) {
    uint32_t bucket_idx = which_bucket(pm, rgb);

    if (!pm->buckets[bucket_idx]) {
        pm->buckets[bucket_idx] = calloc(1, sizeof(struct pm_bucket));
    }
    insert_pm_entry(pm->buckets[bucket_idx], rgb);
}

void p_pixmap(struct pixmap* pm, _Bool print_all_pixels) {
    printf("pixmap with fudge factor of %i and %i buckets\n", pm->fudge_factor, pm->n_buckets);
    for (uint32_t i = 0; i < pm->n_buckets; ++i) {
        if (!pm->buckets[i]) {
            continue;
        }
        printf("  %i: %i pixels\n", i, pm->buckets[i]->n_pixels);
        if (!print_all_pixels) {
            continue;
        }
        for (struct pm_entry* pm_e = pm->buckets[i]->pixels; pm_e; pm_e = pm_e->next) {
            printf("    (%i,%i,%i): %i\n", pm_e->rgb[0], pm_e->rgb[1], pm_e->rgb[2], pm_e->n);
        }
    }
}


/*
 * analyze all pixels, 
 * count number of pixels of each color - make a map!
*/

struct pixmap* pixmap_diff(struct pixmap* pm_a, struct pixmap* pm_b, _Bool additive_only) {
    struct pixmap* pm, * valid_pm;
    int a_tmp, b_tmp;


    if (pm_a->fudge_factor != pm_b->fudge_factor) {
        return NULL;
    }

    pm = malloc(sizeof(struct pixmap));
    init_pixmap(pm, pm_a->fudge_factor);

    for (uint32_t i = 0; i < pm_a->n_buckets; ++i) {
        a_tmp = pm_a->buckets[i] ? pm_a->buckets[i]->n_pixels : 0;
        b_tmp = pm_b->buckets[i] ? pm_b->buckets[i]->n_pixels : 0;


        // TODO: maybe only print if additive or subtractive - only if one entry is 0!
        if (a_tmp != b_tmp) {
            if (additive_only && a_tmp && b_tmp) {
                continue;
            }
            valid_pm = a_tmp ? pm_a : pm_b;

            printf("discrepency found in bucket %i of [%i, %i]\n", i, a_tmp, b_tmp);
            printf("  example value of: (%i,%i,%i)\n", 
                   valid_pm->buckets[i]->pixels->rgb[0], valid_pm->buckets[i]->pixels->rgb[1], valid_pm->buckets[i]->pixels->rgb[2]);

            printf("\033[38;2;%i;%i;%im%s\033[0m\n", 255, 0, 0, "*****");

            /*
             * print out additive or subtractive maybe. OR MAYBE DO also print discrepencies, but i think maybe just when one is nonexistent and
             * one does exist
            */
        }
    }

    return pm;
}

// not sure which parts of this setup must be done multiple times
uint8_t* img_data(char* fn, int* datasz, int* width, int* height) {
    uint8_t* data;
    ILuint img_id;
    /*ilInit();*/
	ilGenImages(1, &img_id);
	ilBindImage(img_id);
	if (!ilLoadImage(fn)) {
        puts("failed to open image");
        return NULL;
	}

    *width = ilGetInteger(IL_IMAGE_WIDTH);
    *height = ilGetInteger(IL_IMAGE_HEIGHT);
    *datasz = *width * *height * 3;
    data = malloc(*datasz);

    ilCopyPixels(0, 0, 0, *width, *height, 1, IL_RGB, IL_UNSIGNED_BYTE, data);
    return data;
}

void build_pixmap(uint8_t* data, int w, int h, struct pixmap* pm) {
    for (int i = 0; i < w * h; ++i) {
        insert_pixmap(pm, data + (i * 3));
        /*printf("data[%i]: [%i,%i,%i]\n", i, data[(i * 3)], data[1 + (i * 3)], data[2 + (i * 3)]);*/
    }
}

struct pixmap* img_to_pixmap(char* fn, uint8_t fudge_factor) {
    struct pixmap* pm = malloc(sizeof(struct pixmap));
    int width, height, datasz;
    uint8_t* data = img_data(fn, &datasz, &width, &height);

    init_pixmap(pm, fudge_factor);

    /*printf("image of size: %i X %i\n", width, height);*/

    build_pixmap(data, width, height, pm);

    return pm;
}

/*write diff finder of two pixmaps! or maybe diff of two datums and build variable fuzziness maps*/
int main(int argc, char* argv[]) {
    struct pixmap* img_a, * img_b;
    ILuint img_id;

    if (argc < 3) {
        return EXIT_FAILURE;
    }

    ilInit();
    /*init_pixmap(&pm, 60);*/
    img_a = img_to_pixmap(argv[1], 10);

    img_b = img_to_pixmap(argv[2], 10);

    /*p_pixmap(pm, 0);*/

    pixmap_diff(img_a, img_b, 1);

	ilDeleteImages(1, &img_id);
}
