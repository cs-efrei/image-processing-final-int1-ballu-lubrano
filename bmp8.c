#include "bmp8.h"
#include <stdio.h>
#include <stdlib.h>

float** create_simple_kernel()
{

    float** kernel = (float**)malloc(3 * sizeof(float*));
    for (int i = 0; i < 3; i++) {
        kernel[i] = (float*)malloc(3 * sizeof(float));
    }


    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = 1.0f/9.0f; // Each cell contributes 1/9 of the value
        }
    }

    return kernel;
}


void free_kernel(float** kernel)
{
    for (int i = 0; i < 3; i++) {
        free(kernel[i]);
    }
    free(kernel);
}

// Load BMP image (simplified with error checking)
t_bmp8* bmp8_loadImage(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return NULL;
    }


    t_bmp8* image = (t_bmp8*)malloc(sizeof(t_bmp8));
    if (!image) {
        printf("Error: Could not allocate memory for image\n");
        fclose(file);
        return NULL;
    }


    if (fread(image->header, 1, 54, file) != 54) {
        printf("Error: Could not read header\n");
        fclose(file);
        free(image);
        return NULL;
    }


    image->width = *(unsigned int*)&image->header[18];
    image->height = *(unsigned int*)&image->header[22];
    image->colorDepth = *(unsigned int*)&image->header[28];
    image->dataSize = *(unsigned int*)&image->header[34];


    if (image->colorDepth != 8) {
        printf("Error: Image is not 8-bit grayscale\n");
        fclose(file);
        free(image);
        return NULL;
    }


    if (fread(image->colorTable, 1, 1024, file) != 1024) {
        printf("Error: Could not read color table\n");
        fclose(file);
        free(image);
        return NULL;
    }


    image->data = (unsigned char*)malloc(image->dataSize);
    if (!image->data) {
        printf("Error: Could not allocate memory for pixel data\n");
        fclose(file);
        free(image);
        return NULL;
    }

    if (fread(image->data, 1, image->dataSize, file) != image->dataSize) {
        printf("Error: Could not read pixel data\n");
        fclose(file);
        free(image->data);
        free(image);
        return NULL;
    }

    fclose(file);
    return image;
}


void bmp8_saveImage(const char* filename, t_bmp8* image)
{
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Could not create file %s\n", filename);
        return;
    }
   

    //header54 bytes
    fwrite(image->header, 1, 54, file);

    //color table 1024 bytes
    fwrite(image->colorTable, 1, 1024, file);

    //pixel data
    fwrite(image->data, 1, image->dataSize, file);

    fclose(file);
}


void bmp8_free(t_bmp8* image)
{
    if (image) {
        if (image->data) {
            free(image->data);
        }
        free(image);
    }
}


void bmp8_printInfo(t_bmp8* image)
{
    printf("Image Information:\n");
    printf("Width: %d pixels\n", image->width);
    printf("Height: %d pixels\n", image->height);
    printf("Color Depth: %d bits per pixel\n", image->colorDepth);
    printf("Data Size: %d bytes\n", image->dataSize);
}


void bmp8_negative(t_bmp8* image)
{
    for (unsigned int i = 0; i < image->dataSize; i++) {
        image->data[i] = 255 - image->data[i];
    }
}


void bmp8_brightness(t_bmp8* image, int change)
{
    for (unsigned int i = 0; i < image->dataSize; i++) {
        int new_value = image->data[i] + change;


        if (new_value > 255) new_value = 255;
        if (new_value < 0) new_value = 0;

        image->data[i] = (unsigned char)new_value;
    }
}


void bmp8_threshold(t_bmp8* image, int threshold)
{
    for (unsigned int i = 0; i < image->dataSize; i++) {

        //if brighter than threshold it make it white
        image->data[i] = (image->data[i] >= threshold) ? 255 : 0;
    }
}


void bmp8_applyFilter(t_bmp8* image, float** kernel)
{

    unsigned char* new_data = (unsigned char*)malloc(image->dataSize);
    if (!new_data) {
        printf("Error: Could not allocate memory for filter\n");
        return;
    }


    memcpy(new_data, image->data, image->dataSize);


    for (unsigned int y = 1; y < image->height - 1; y++) {
        for (unsigned int x = 1; x < image->width - 1; x++) {
            float sum = 0.0f;


            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    unsigned int pixel_pos = (y + ky) * image->width + (x + kx);
                    sum += image->data[pixel_pos] * kernel[ky + 1][kx + 1];
                }
            }


            unsigned int pos = y * image->width + x;
            if (sum > 255) sum = 255;
            if (sum < 0) sum = 0;
            new_data[pos] = (unsigned char)sum;
        }
    }


    memcpy(image->data, new_data, image->dataSize);
    free(new_data);
}
float** create_blur_kernel()
{
    //alloc memory for 3 rows
    float** kernel = (float**)malloc(3 * sizeof(float*));

    //in each row
    for (int i = 0; i < 3; i++) {
        kernel[i] = (float*)malloc(3 * sizeof(float));
    }

    //blur values each cell = 1/9
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = 1.0f / 9.0f;
        }
    }

    return kernel;
}
void bmp8_applyBlur(t_bmp8* image)
{
    float** kernel = create_blur_kernel();
    unsigned char* new_data = (unsigned char*)malloc(image->dataSize);

    if (!new_data) {
        printf("Error: Could not allocate memory for filter\n");
        free_kernel(kernel);
        return;
    }


    memcpy(new_data, image->data, image->dataSize);


    for (unsigned int y = 1; y < image->height - 1; y++) {
        for (unsigned int x = 1; x < image->width - 1; x++) {
            float sum = 0.0f;


            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    unsigned int pos = (y + ky) * image->width + (x + kx);
                    sum += image->data[pos] * kernel[ky + 1][kx + 1];
                }
            }


            unsigned int pos = y * image->width + x;
            if (sum > 255) sum = 255;
            if (sum < 0) sum = 0;
            new_data[pos] = (unsigned char)sum;
        }
    }


    memcpy(image->data, new_data, image->dataSize);


    free(new_data);
    free_kernel(kernel);
}
