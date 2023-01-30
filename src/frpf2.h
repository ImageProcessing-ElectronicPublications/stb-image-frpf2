/*
https://github.com/ImageProcessing-ElectronicPublications/imthreshold

Find Replicant Pixel Fast

*/

#ifndef FRPF2_H_
#define FRPF2_H_

#define FRPF2_VERSION "1.2"

#ifdef FRPF2_STATIC
#define FRPF2API static
#else
#define FRPF2API extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

FRPF2API void ImageResizeMean2 (unsigned char *src, int height, int width, int channels, unsigned char *res);
FRPF2API void ImageGradient (unsigned char *src, int height, int width, int channels, int *res);
FRPF2API void ImageFRPF2 (unsigned char *src, unsigned char *frp, int *gsrc, int *gfrp, int height, int width, int channels, int radius, int threshold, float mixpix, unsigned char *res);

#ifdef __cplusplus
}
#endif

#ifdef FRPF2_IMPLEMENTATION

/*
ImageResizeMean2()

input:
buf - unsigned char* image (height * width * channels)

output:
res - unsigned char* image (height/2 * width/2 * channels)

Use:
ImageResizeMean2(buf, width, height, channels, res);
*/

FRPF2API void ImageResizeMean2 (unsigned char *src, int height, int width, int channels, unsigned char *res)
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
                n = (n > 0) ? n : 1;
                sum = (sum + n / 2) / n;
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
res - int* gradient (height * width * channels * 4)

Use:
ImageGradient(buf, width, height, channels, res);
*/

FRPF2API void ImageGradient (unsigned char *src, int height, int width, int channels, int *res)
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
                    }
                }
                res[k + k + k + k] = ims;
                ims = 0;
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
                        ims += i * im;
                    }
                }
                res[k + k + k + k + 1] = ims;
                ims = 0;
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
                        ims += j * im;
                    }
                }
                res[k + k + k + k + 2] = ims;
                ims = 0;
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
                        ims += i * j * im;
                    }
                }
                res[k + k + k + k + 3] = ims;
                k++;
            }
        }
    }
}

/*
ImageFRPF2()

input:
buf - unsigned char* image (height * width * channels)
frp - unsigned char* image (height/2 * width/2 * channels)
gscr - int* gradient (height * width * channels * 4)
gfrp - int* gradient (height/2 * width/2 * channels * 4)
radius - radius find (deault 0 - all)
threshold - threshold find (default 0)
mixpix - mix pixel (default 0.7f)

output:
res - int* gradient (height * width * channels)

Use:
ImageFRP2(buf, frp, gscr, gfrp, width, height, channels, radius, threshold, mixpix, res);
*/

FRPF2API void ImageFRPF2 (unsigned char *src, unsigned char *frp, int *gsrc, int *gfrp, int height, int width, int channels, int radius, int threshold, float mixpix, unsigned char *res)
{
    unsigned int y, x, d, l, heightt, widtht, heightf, widthf;
    int y0, x0, y1, x1;
    int xf, yf, xp, yp, xs, ys, xt, yt, im0, img, im, imp, imd;
    int g, gx, gy, gz, gf, gfx, gfy, gfz, gd, gdmin, gs;
    int ga, gxa, gya, gza, gfa, gfxa, gfya, gfza;
    int z, zy, zx, zz[4];
    size_t k, kf, kfl, kp, ks, kt, line, linep, linet;

    if (radius < 0) return;
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
        y0 = y / 2 - radius;
        y1 = (y + 1) / 2 + radius + 1;
        y0 = (radius == 0) ? 0 : (y0 < 0) ? 0 : (y0 < heightf) ? y0 : heightf;
        y1 = (radius == 0) ? heightf : (y1 < 0) ? 0 : (y1 < heightf) ? y1 : heightf;
        for (x = 0; x < width; x++)
        {
            x0 = x / 2 - radius;
            x1 = (x + 1) / 2 + radius + 1;
            x0 = (radius == 0) ? 0 : (x0 < 0) ? 0 : (x0 < widthf) ? x0 : widthf;
            x1 = (radius == 0) ? widthf : (x1 < 0) ? 0 : (x1 < widthf) ? x1 : widthf;
            for (d = 0; d < channels; d++)
            {
                im0 = src[k];
                gdmin = 1048576.0f;
                kp = 0;
                g = gsrc[k + k + k + k];
                gy = gsrc[k + k + k + k + 1];
                gx = gsrc[k + k + k + k + 2];
                gz = gsrc[k + k + k + k + 3];
                ga = (g < 0) ? -g : g;
                gya = (gy < 0) ? -gy : gy;
                gxa = (gx < 0) ? -gx : gx;
                gza = (gz < 0) ? -gz : gz;
                kf = 0;
                for (yf = y0; yf < y1; yf++)
                {
                    kfl = (widthf * yf + x0) * channels + d;
                    kf = kfl;
                    for (xf = x0; xf < x1; xf++)
                    {
                        gs = 0;
                        gf = gfrp[kf + kf + kf + kf];
                        gfy = gfrp[kf + kf + kf + kf + 1];
                        gfx = gfrp[kf + kf + kf + kf + 2];
                        gfz = gfrp[kf + kf + kf + kf + 3];
                        gfa = (gf < 0) ? -gf : gf;
                        gfya = (gfy < 0) ? -gfy : gfy;
                        gfxa = (gfx < 0) ? -gfx : gfx;
                        gfza = (gfz < 0) ? -gfz : gfz;
                        gd = (ga > gfa) ? (ga - gfa) : (gfa - ga);
                        gs += gd;
                        gd = (gya > gfya) ? (gya - gfya) : (gfya - gya);
                        gs += gd;
                        gd = (gxa > gfxa) ? (gxa - gfxa) : (gfxa - gxa);
                        gs += gd;
                        gd = (gza > gfza) ? (gza - gfza) : (gfza - gza);
                        gs += gd;

                        if (gs < gdmin)
                        {
                            gdmin = gs;
                            z = g * gf;
                            zy = gy * gfy;
                            zx = gx * gfx;
                            yp = yf;
                            xp = xf;
                            kp = kf;
                        }
                        kf += channels;
                        if (gs <= threshold) break;
                    }
                    if (gs <= threshold) break;
                }
                //yp = kp / linep;
                //xp = (kp - yp * linep) / channels;

                kf = kp;
                imp = frp[kp];
                imd = im0 - imp;

                xs = xp + xp;
                ys = yp + yp;
                ks = (width * ys + xs) * channels + d;
                xt = x + x;
                yt = y + y;
                kt = (widtht * yt + xt) * channels + d;

                zz[0] = src[ks];
                zz[1] = src[ks + channels];
                zz[2] = src[ks + line];
                zz[3] = src[ks + line + channels];
                for (l = 0; l < 4; l++)
                {
                    zz[l] -= imp;
                    zz[l] = (int)((float)zz[l] * mixpix + 0.5f);
                }
                if (z < 0)
                {
                    for (l = 0; l < 4; l++)
                    {
                        zz[l] = -zz[l];
                    }
                    zy = -zy;
                    zx = -zx;
                }
                for (l = 0; l < 4; l++)
                {
                    zz[l] += im0;
                    zz[l] = (zz[l] < 0) ? 0 : (zz[l] < 255) ? zz[l] : 255;
                }

                if (zy < 0)
                {
                    if (zx < 0)
                    {
                        res[kt] = zz[3];
                        res[kt + channels] = zz[2];
                        res[kt + linet] = zz[1];
                        res[kt + linet + channels] = zz[0];
                    }
                    else
                    {
                        res[kt] = zz[2];
                        res[kt + channels] = zz[3];
                        res[kt + linet] = zz[0];
                        res[kt + linet + channels] = zz[1];
                    }
                }
                else
                {
                    if (zx < 0)
                    {
                        res[kt] = zz[1];
                        res[kt + channels] = zz[0];
                        res[kt + linet] = zz[3];
                        res[kt + linet + channels] = zz[2];
                    }
                    else
                    {
                        res[kt] = zz[0];
                        res[kt + channels] = zz[1];
                        res[kt + linet] = zz[2];
                        res[kt + linet + channels] = zz[3];
                    }
                }
                k++;
            }
        }
    }
}

#endif /* FRPF2_IMPLEMENTATION */

#endif /* FRPF2_H_ */
