#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include "frpf2.h"

void frp2_usage(char* prog, int radius, int threshold, float mixpix)
{
    printf("StbFRPF2 version %s.\n", FRPF2_VERSION);
    printf("usage: %s [options] image_in out.png\n", prog);
    printf("options:\n");
    printf("  -m N.N    mix pixel (default %f)\n", mixpix);
    printf("  -r NUM    radius find (0 - all, default %d)\n", radius);
    printf("  -t NUM    threshold find (default %d)\n", threshold);
    printf("  -h        show this help message and exit\n");
}

int main(int argc, char **argv)
{
    unsigned int height, width, channels, y, x, d;
    unsigned int heightr, widthr, heightf, widthf;
    int radius = 0;
    int threshold = 0;
    float mixpix = 0.75f;
    int fhelp = 0;
    int opt;
    size_t ki, kd;
    unsigned char *data = NULL, *resize = NULL, *frp = NULL;
    int *gdata = NULL, *gfrp = NULL;
    stbi_uc *img = NULL;

    while ((opt = getopt(argc, argv, ":m:r:t:h")) != -1)
    {
        switch(opt)
        {
        case 'm':
            mixpix = atof(optarg);
            break;
        case 'r':
            radius = atoi(optarg);
            if (radius < 0)
            {
                fprintf(stderr, "ERROR: bad radius: %d\n", radius);
                return 2;
            }
            break;
        case 't':
            threshold = atoi(optarg);
            if (threshold < 0)
            {
                fprintf(stderr, "ERROR: bad threshold: %d\n", threshold);
                return 2;
            }
            break;
        case 'h':
            fhelp = 1;
            break;
        case ':':
            fprintf(stderr, "ERROR: option needs a value\n");
            return 2;
            break;
        case '?':
            fprintf(stderr, "ERROR: unknown option: %c\n", optopt);
            return 3;
            break;
        }
    }
    printf("\n");
    if(optind + 2 > argc || fhelp)
    {
        frp2_usage(argv[0], radius, threshold, mixpix);
        return 0;
    }
    const char *src_name = argv[optind];
    const char *dst_name = argv[optind + 1];


    printf("Load: %s\n", src_name);
    if (!(img = stbi_load(src_name, &width, &height, &channels, STBI_rgb_alpha)))
    {
        fprintf(stderr, "ERROR: not read image: %s\n", src_name);
        return 1;
    }
    printf("image: %dx%d:%d\n", width, height, channels);
    heightr = height * 2;
    widthr = width * 2;
    heightf = height / 2;
    widthf = width / 2;
    if (!(data = (unsigned char*)malloc(height * width * channels * sizeof(unsigned char))))
    {
        fprintf(stderr, "ERROR: not use memmory\n");
        return 1;
    }
    if (!(resize = (unsigned char*)malloc(heightr * widthr * channels * sizeof(unsigned char))))
    {
        fprintf(stderr, "ERROR: not use memmory\n");
        return 1;
    }
    if (!(frp = (unsigned char*)malloc(heightf * widthf * channels * sizeof(unsigned char))))
    {
        fprintf(stderr, "ERROR: not use memmory\n");
        return 1;
    }
    if (!(gdata = (int*)malloc(height * width * channels * 4 * sizeof(int))))
    {
        fprintf(stderr, "ERROR: not use memmory\n");
        return 1;
    }
    if (!(gfrp = (int*)malloc(heightf * widthf * channels * 4 * sizeof(int))))
    {
        fprintf(stderr, "ERROR: not use memmory\n");
        return 1;
    }
    ki = 0;
    kd = 0;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            for (d = 0; d < channels; d++)
            {
                data[kd + d] = (unsigned char)img[ki + d];
            }
            ki += STBI_rgb_alpha;
            kd += channels;
        }
    }
    stbi_image_free(img);

    printf("process: downsample\n");
    ImageResizeMean2 (data, height, width, channels, frp);
    printf("process: gradient\n");
    ImageGradient (data, height, width, channels, gdata);
    ImageGradient (frp, heightf, widthf, channels, gfrp);
    printf("process: FRPF2\n");
    if (radius == 0)
        printf(" region: all\n");
    else
        printf(" region: %dx%d\n", (2 * radius + 1), (2 * radius + 1));
    printf(" threshold: %d\n", threshold);
    printf(" mix: %f\n", mixpix);
    ImageFRPF2 (data, frp, gdata, gfrp, height, width, channels, radius, threshold, mixpix, resize);

    printf("Save png: %s\n", dst_name);
    if (!(stbi_write_png(dst_name, widthr, heightr, channels, resize, widthr * channels)))
    {
        fprintf(stderr, "ERROR: not write image: %s\n", dst_name);
        return 1;
    }

    free(gfrp);
    free(gdata);
    free(resize);
    free(frp);
    free(data);
    return 0;
}
