int imagetotga(opj_image_t * image, const char *outfile)
{
    int width, height, bpp, x, y;
    OPJ_BOOL write_alpha;
    unsigned int i;
    int adjustR, adjustG = 0, adjustB = 0, fails;
    unsigned int alpha_channel;
    float r, g, b, a;
    unsigned char value;
    float scale;
    FILE *fdest;
    size_t res;
    fails = 1;

    fdest = fopen(outfile, "wb");
    if (!fdest) {
        fprintf(stderr, "ERROR -> failed to open %s for writing\n", outfile);
        return 1;
    }

    for (i = 0; i < image->numcomps - 1; i++) {
        if ((image->comps[0].dx != image->comps[i + 1].dx)
                || (image->comps[0].dy != image->comps[i + 1].dy)
                || (image->comps[0].prec != image->comps[i + 1].prec)
                || (image->comps[0].sgnd != image->comps[i + 1].sgnd)) {
            fclose(fdest);
            fprintf(stderr,
                    "Unable to create a tga file with such J2K image charateristics.\n");
            return 1;
        }
    }

    width  = (int)image->comps[0].w;
    height = (int)image->comps[0].h;

    /* Mono with alpha, or RGB with alpha. */
    write_alpha = (image->numcomps == 2) || (image->numcomps == 4);

    /* Write TGA header  */
    bpp = write_alpha ? 32 : 24;

    if (!tga_writeheader(fdest, bpp, width, height, OPJ_TRUE)) {
        goto fin;
    }

    alpha_channel = image->numcomps - 1;

    scale = 255.0f / (float)((1 << image->comps[0].prec) - 1);

    adjustR = (image->comps[0].sgnd ? 1 << (image->comps[0].prec - 1) : 0);
    if (image->numcomps >= 3) {
        adjustG = (image->comps[1].sgnd ? 1 << (image->comps[1].prec - 1) : 0);
        adjustB = (image->comps[2].sgnd ? 1 << (image->comps[2].prec - 1) : 0);
    }

    for (y = 0; y < height; y++) {
        unsigned int index = (unsigned int)(y * width);

        for (x = 0; x < width; x++, index++) {
            r = (float)(image->comps[0].data[index] + adjustR);

            if (image->numcomps > 2) {
                g = (float)(image->comps[1].data[index] + adjustG);
                b = (float)(image->comps[2].data[index] + adjustB);
            } else {
                /* Greyscale ... */
                g = r;
                b = r;
            }

            /* TGA format writes BGR ... */
            if (b > 255.) {
                b = 255.;
            } else if (b < 0.) {
                b = 0.;
            }
            value = (unsigned char)(b * scale);
            res = fwrite(&value, 1, 1, fdest);

            if (res < 1) {
                fprintf(stderr, "failed to write 1 byte for %s\n", outfile);
                goto fin;
            }
            if (g > 255.) {
                g = 255.;
            } else if (g < 0.) {
                g = 0.;
            }
            value = (unsigned char)(g * scale);
            res = fwrite(&value, 1, 1, fdest);

            if (res < 1) {
                fprintf(stderr, "failed to write 1 byte for %s\n", outfile);
                goto fin;
            }
            if (r > 255.) {
                r = 255.;
            } else if (r < 0.) {
                r = 0.;
            }
            value = (unsigned char)(r * scale);
            res = fwrite(&value, 1, 1, fdest);

            if (res < 1) {
                fprintf(stderr, "failed to write 1 byte for %s\n", outfile);
                goto fin;
            }

            if (write_alpha) {
                a = (float)(image->comps[alpha_channel].data[index]);
                if (a > 255.) {
                    a = 255.;
                } else if (a < 0.) {
                    a = 0.;
                }
                value = (unsigned char)(a * scale);
                res = fwrite(&value, 1, 1, fdest);

                if (res < 1) {
                    fprintf(stderr, "failed to write 1 byte for %s\n", outfile);
                    goto fin;
                }
            }
        }
    }
    fails = 0;
fin:
    fclose(fdest);

    return fails;
}
