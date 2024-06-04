#include <stdio.h>
#include <stdlib.h>

int min(int a, int b) {
    return (a < b) ? a : b;
}

int main() {
    const char* filename = "mandelbrot.ppm";
    printf("P3\n");

    int size = 4;
    int pixel = 1000;
    printf("%d %d\n", pixel, pixel);
    printf("255\n");

    for (int i = 0; i < pixel; i++) {
        double x = i * size / (double)pixel - size / 2.0;
        for (int j = 0; j < pixel; j++) {
            double y = j * size / (double)pixel - size / 2.0;
            double a = 0.0;
            double b = 0.0;
            int d = 0;
            int k;
            for (k = 0; k < 50; k++) {
                double _a = a * a - b * b + x;
                double _b = 2 * a * b + y;
                a = _a;
                b = _b;
                if (a * a + b * b > 4) {
                    d = 1;
                    break;
                }
            }
            if (d) {
                int color = min(k * 12, 255);
                printf("%d 0 0 ", color);
            } else {
                printf("255 255 255 ");
            }
        }
        printf("\n");
    }

    return EXIT_SUCCESS;
}

