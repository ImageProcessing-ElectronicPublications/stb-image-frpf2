/*
https://github.com/ImageProcessing-ElectronicPublications/imthreshold

Find Replicant Pixel

*/

#ifndef FRP2_H_
#define FRP2_H_

#define FRP2_VERSION "1.0"

#ifdef FRP2_STATIC
#define FRP2API static
#else
#define FRP2API extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

FRP2API void ImageResizeMean2 (unsigned char *src, int height, int width, int channels, unsigned char *res);
FRP2API void ImageGradient (unsigned char *src, int height, int width, int channels, int *res);
FRP2API void ImageFRP2 (unsigned char *src, unsigned char *frp, int *gsrc, int *gfrp, int height, int width, int channels, unsigned char *res);

#ifdef __cplusplus
}
#endif

#ifdef FRP2_IMPLEMENTATION

/*
ImageResizeMean2()

input:
buf - unsigned char* image (height * width * channels)

output:
res - unsigned char* image (height/2 * width/2 * channels)

Use:
ImageResizeMean2(buf, width, height, channels, res);
*/

FRP2API void ImageResizeMean2 (unsigned char *src, int height, int width, int channels, unsigned char *res)
{
    unsigned int y, x, d, y0, x0, y1, x1, yf, xf;
    size_t k, kf, sum, n, line;

    unsigned int widthr = width / 2;
    unsigned int heightr = height / 2;
    line = width * channels;
    for (d = 0; d < channels; d++)
    {
        k = d;
        for (y = 0; y < heightr; y++)
        {
            y0 = y * 2;
            y1 = y0 + 2;
            y1 = (y1 < height) ? y1 : height;
            for (x = 0; x < widthr; x++)
            {
                x0 = x * 2;
                x1 = x0 + 2;
                x1 = (x1 < width) ? x1 : width;
                sum = 0;
                n = 0;
                for (yf = y0; yf < y1; yf++)
                {
                    for (xf = x0; xf < x1; xf++)
                    {
                        kf = (yf * width + xf) * channels + d;
                        sum += (size_t)src[kf];
                        n++;
                    }
                }
                sum = (n > 0) ? ((sum + n / 2) / n) : 0;
                res[k] = (unsigned char)((sum < 0) ? 0 : (sum < 255) ? sum : 255);
                k += channels;
            }
        }
    }
}

/*
ImageGradient()

input:
buf - unsigned char* image (height * width * channels)

output:
res - int* gradient (height * width * channels * 9)

Use:
ImageGradient(buf, width, height, channels, res);
*/

FRP2API void ImageGradient (unsigned char *src, int height, int width, int channels, int *res)
{
    int y, x, d, i, j, l, yf, xf, im0, im, ims;
    size_t k, kf, line;

    line = width * channels;
    k = 0;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            for (d = 0; d < channels; d++)
            {
                im0 = (int)src[k];
                ims = 9 * im0;
                l = 0;
                for (i = -1; i <= 1; i++)
                {
                    yf = y + i;
                    yf = (yf < 0) ? 0 : (yf < height) ? yf : (height - 1);
                    for (j = -1; j <= 1; j++)
                    {
                        xf = x + j;
                        xf = (xf < 0) ? 0 : (xf < width) ? xf : (width - 1);
                        kf = (width * yf + xf) * channels + d;
                        im = (int)src[kf];
                        ims -= im;
                        res[k * 9 + l] = (im0 - im);
                        l++;
                    }
                }
                res[k * 9 + 4] = ims;
                k++;
            }
        }
    }
}

/*
ImageFRP2()

input:
buf - unsigned char* image (height * width * channels)
frp - unsigned char* image (height/2 * width/2 * channels)
gscr - int* gradient (height * width * channels * 9)

output:
res - int* gradient (height * width * channels)

Use:
ImageFRP2(buf, width, height, channels, res);
*/

FRP2API void ImageFRP2 (unsigned char *src, unsigned char *frp, int *gsrc, int *gfrp, int height, int width, int channels, unsigned char *res)
{
    unsigned int y, x, d, l, heightt, widtht, heightf, widthf;
    int xf, yf, xp, yp, xs, ys, xt, yt, im0, img, im, imp, imd;
    float g, gf, gd, gdmin, gs, gsv;
    int zy, zx, zfy, zfx;
    size_t k, kf, kp, ks, kt, line, linep, linet;

    heightt = height * 2;
    widtht = width * 2;
    heightf = height / 2;
    widthf = width / 2;
    line = width * channels;
    linep = widthf * channels;
    linet = widtht * channels;
    k = 0;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            for (d = 0; d < channels; d++)
            {
                im0 = src[k];
                gdmin = 1048576.0f;
                kp = 0;
                ks = k * 9;
                for (kf = d; kf < (linep * heightf); kf += channels)
                {
                    gs = 0;
                    gsv = 0;
                    kt = kf * 9;
                    for (l = 0; l < 9; l++)
                    {
                        g = gsrc[ks + l];
                        gf = gfrp[kt + l];
                        gd = (g > gf) ? (g - gf) : (gf - g);
                        gsv += gd;
                    }
                    if (gsv < gdmin)
                    {
                        gdmin = gsv;
                        kp = kf;
                    }
                    if (gsv == 0) break;
                }
                yp = kp / linep;
                xp = (kp - yp * linep) / channels;

                kf = kp;
                imp = frp[kp];
                imd = im0 - imp;

                xs = xp + xp;
                ys = yp + yp;
                ks = (width * ys + xs) * channels + d;
                xt = x + x;
                yt = y + y;
                kt = (widtht * yt + xt) * channels + d;

                im = src[ks] + imd;
                im = (im < 0) ? 0 : (im < 255) ? im : 255;
                res[kt] = im;
                im = src[ks + channels] + imd;
                im = (im < 0) ? 0 : (im < 255) ? im : 255;
                res[kt + channels] = im;
                im = src[ks + line] + imd;
                im = (im < 0) ? 0 : (im < 255) ? im : 255;
                res[kt + linet] = im;
                im = src[ks + line + channels] + imd;
                im = (im < 0) ? 0 : (im < 255) ? im : 255;
                res[kt + linet + channels] = im;
                k++;
            }
        }
    }
}

#endif /* FRP2_IMPLEMENTATION */

#endif /* FRP2_H_ */
