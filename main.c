#include "bmp8.h"
#include <stdio.h>

int main()
{
    t_bmp8* image = NULL;
    char filename[100];
    int choice, value;

    printf("\n-----|Image Processor|-----\n");

    while (1) {
        printf("\nMain Menu:\n");
        printf("1. Load image\n");
        printf("2. Save image\n");
        printf("3. Create negative\n");
        printf("4. Adjust brightness\n");
        printf("5. Apply threshold\n");
        printf("6. Apply blur\n");
        printf("7. Show image info\n");
        printf("8. Exit\n");
        printf("Your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                if (image) {
                    bmp8_free(image);
                    image = NULL;
                }
                printf("Enter filename: ");
                scanf("%99s", filename);
                image = bmp8_loadImage(filename);  
                if (image) {
                    printf("Image loaded successfully!\n");
                } else {
                    printf("Failed to load image. Check if file exists.\n");
                }
                break;
            case 2:
                if (!image) {
                    printf("No image loaded!\n");
                    break;
                }
                printf("Enter filename to save: ");
                scanf("%99s", filename);
                bmp8_saveImage(filename, image);
                printf("Image saved successfully!\n");
                break;
            case 3:
                if (!image) {
                    printf("No image loaded!\n");
                    break;
                }
                bmp8_negative(image);
                printf("Negative created!\n");
                break;
            case 8:
                if (image) bmp8_free(image);
                printf("Goodbye!\n");
                return 0;

            default:
                printf("Invalid choice!\n");
        }
    }
}
