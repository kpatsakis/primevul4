opj_image_t* pnmtoimage(const char *filename, opj_cparameters_t *parameters)
{
    int subsampling_dx = parameters->subsampling_dx;
    int subsampling_dy = parameters->subsampling_dy;

    FILE *fp = NULL;
    int i, compno, numcomps, w, h, prec, format;
    OPJ_COLOR_SPACE color_space;
    opj_image_cmptparm_t cmptparm[4]; /* RGBA: max. 4 components */
    opj_image_t * image = NULL;
    struct pnm_header header_info;

    if ((fp = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "pnmtoimage:Failed to open %s for reading!\n", filename);
        return NULL;
    }
    memset(&header_info, 0, sizeof(struct pnm_header));

    read_pnm_header(fp, &header_info);

    if (!header_info.ok) {
        fclose(fp);
        return NULL;
    }

    /* This limitation could be removed by making sure to use size_t below */
    if (header_info.height != 0 &&
            header_info.width > INT_MAX / header_info.height) {
        fprintf(stderr, "pnmtoimage:Image %dx%d too big!\n",
                header_info.width, header_info.height);
        fclose(fp);
        return NULL;
    }

    format = header_info.format;

    switch (format) {
    case 1: /* ascii bitmap */
    case 4: /* raw bitmap */
        numcomps = 1;
        break;

    case 2: /* ascii greymap */
    case 5: /* raw greymap */
        numcomps = 1;
        break;

    case 3: /* ascii pixmap */
    case 6: /* raw pixmap */
        numcomps = 3;
        break;

    case 7: /* arbitrary map */
        numcomps = header_info.depth;
        break;

    default:
        fclose(fp);
        return NULL;
    }
    if (numcomps < 3) {
        color_space = OPJ_CLRSPC_GRAY;    /* GRAY, GRAYA */
    } else {
        color_space = OPJ_CLRSPC_SRGB;    /* RGB, RGBA */
    }

    prec = has_prec(header_info.maxval);

    if (prec < 8) {
        prec = 8;
    }

    w = header_info.width;
    h = header_info.height;
    subsampling_dx = parameters->subsampling_dx;
    subsampling_dy = parameters->subsampling_dy;

    memset(&cmptparm[0], 0, (size_t)numcomps * sizeof(opj_image_cmptparm_t));

    for (i = 0; i < numcomps; i++) {
        cmptparm[i].prec = (OPJ_UINT32)prec;
        cmptparm[i].bpp = (OPJ_UINT32)prec;
        cmptparm[i].sgnd = 0;
        cmptparm[i].dx = (OPJ_UINT32)subsampling_dx;
        cmptparm[i].dy = (OPJ_UINT32)subsampling_dy;
        cmptparm[i].w = (OPJ_UINT32)w;
        cmptparm[i].h = (OPJ_UINT32)h;
    }
    image = opj_image_create((OPJ_UINT32)numcomps, &cmptparm[0], color_space);

    if (!image) {
        fclose(fp);
        return NULL;
    }

    /* set image offset and reference grid */
    image->x0 = (OPJ_UINT32)parameters->image_offset_x0;
    image->y0 = (OPJ_UINT32)parameters->image_offset_y0;
    image->x1 = (OPJ_UINT32)(parameters->image_offset_x0 + (w - 1) * subsampling_dx
                             + 1);
    image->y1 = (OPJ_UINT32)(parameters->image_offset_y0 + (h - 1) * subsampling_dy
                             + 1);

    if ((format == 2) || (format == 3)) { /* ascii pixmap */
        unsigned int index;

        for (i = 0; i < w * h; i++) {
            for (compno = 0; compno < numcomps; compno++) {
                index = 0;
                if (fscanf(fp, "%u", &index) != 1) {
                    fprintf(stderr,
                            "\nWARNING: fscanf return a number of element different from the expected.\n");
                }

                image->comps[compno].data[i] = (OPJ_INT32)(index * 255) / header_info.maxval;
            }
        }
    } else if ((format == 5)
               || (format == 6)
               || ((format == 7)
                   && (header_info.gray || header_info.graya
                       || header_info.rgb || header_info.rgba))) { /* binary pixmap */
        unsigned char c0, c1, one;

        one = (prec < 9);

        for (i = 0; i < w * h; i++) {
            for (compno = 0; compno < numcomps; compno++) {
                if (!fread(&c0, 1, 1, fp)) {
                    fprintf(stderr,
                            "\nError: fread return a number of element different from the expected.\n");
                    opj_image_destroy(image);
                    fclose(fp);
                    return NULL;
                }
                if (one) {
                    image->comps[compno].data[i] = c0;
                } else {
                    if (!fread(&c1, 1, 1, fp)) {
                        fprintf(stderr,
                                "\nError: fread return a number of element different from the expected.\n");
                    }
                    /* netpbm: */
                    image->comps[compno].data[i] = ((c0 << 8) | c1);
                }
            }
        }
    } else if (format == 1) { /* ascii bitmap */
        for (i = 0; i < w * h; i++) {
            unsigned int index;

            if (fscanf(fp, "%u", &index) != 1) {
                fprintf(stderr,
                        "\nWARNING: fscanf return a number of element different from the expected.\n");
            }

            image->comps[0].data[i] = (index ? 0 : 255);
        }
    } else if (format == 4) {
        int x, y, bit;
        unsigned char uc;

        i = 0;
        for (y = 0; y < h; ++y) {
            bit = -1;
            uc = 0;

            for (x = 0; x < w; ++x) {
                if (bit == -1) {
                    bit = 7;
                    uc = (unsigned char)getc(fp);
                }
                image->comps[0].data[i] = (((uc >> bit) & 1) ? 0 : 255);
                --bit;
                ++i;
            }
        }
    } else if ((format == 7 && header_info.bw)) { /*MONO*/
        unsigned char uc;

        for (i = 0; i < w * h; ++i) {
            if (!fread(&uc, 1, 1, fp)) {
                fprintf(stderr,
                        "\nError: fread return a number of element different from the expected.\n");
            }
            image->comps[0].data[i] = (uc & 1) ? 0 : 255;
        }
    }
    fclose(fp);

    return image;
}/* pnmtoimage() */
