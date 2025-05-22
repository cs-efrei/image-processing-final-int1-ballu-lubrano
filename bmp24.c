#include "bmp24.h"
#include <stdio.h>
#include <stdlib.h>

t_pixel ** bmp24_allocateDataPixels (int width, int height) {
    t_pixel **pixels = malloc(height * sizeof(t_pixel *));
        if (pixels == NULL) return NULL;

        for (int i = 0; i < height; i++) {
            pixels[i] = malloc(width * sizeof(t_pixel));
            if (pixels[i] == NULL) {
                for (int j = 0; j < i; j++) {
                    free(pixels[j]);
                }
                free(pixels);
                return NULL;
            }
        }

    return pixels;
    }


void bmp24_freeDataPixels (t_pixel ** pixels, int height){


}