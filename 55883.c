static opj_image_t* rawtoimage_common(const char *filename,
                                      opj_cparameters_t *parameters, raw_cparameters_t *raw_cp, OPJ_BOOL big_endian)
{
    int subsampling_dx = parameters->subsampling_dx;
    int subsampling_dy = parameters->subsampling_dy;

    FILE *f = NULL;
    int i, compno, numcomps, w, h;
    OPJ_COLOR_SPACE color_space;
    opj_image_cmptparm_t *cmptparm;
    opj_image_t * image = NULL;
    unsigned short ch;

    if ((!(raw_cp->rawWidth & raw_cp->rawHeight & raw_cp->rawComp &
            raw_cp->rawBitDepth)) == 0) {
        fprintf(stderr, "\nError: invalid raw image parameters\n");
        fprintf(stderr, "Please use the Format option -F:\n");
        fprintf(stderr,
                "-F <width>,<height>,<ncomp>,<bitdepth>,{s,u}@<dx1>x<dy1>:...:<dxn>x<dyn>\n");
        fprintf(stderr,
                "If subsampling is omitted, 1x1 is assumed for all components\n");
        fprintf(stderr,
                "Example: -i image.raw -o image.j2k -F 512,512,3,8,u@1x1:2x2:2x2\n");
        fprintf(stderr, "         for raw 512x512 image with 4:2:0 subsampling\n");
        fprintf(stderr, "Aborting.\n");
        return NULL;
    }

    f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Failed to open %s for reading !!\n", filename);
        fprintf(stderr, "Aborting\n");
        return NULL;
    }
    numcomps = raw_cp->rawComp;

    /* FIXME ADE at this point, tcp_mct has not been properly set in calling function */
    if (numcomps == 1) {
        color_space = OPJ_CLRSPC_GRAY;
    } else if ((numcomps >= 3) && (parameters->tcp_mct == 0)) {
        color_space = OPJ_CLRSPC_SYCC;
    } else if ((numcomps >= 3) && (parameters->tcp_mct != 2)) {
        color_space = OPJ_CLRSPC_SRGB;
    } else {
        color_space = OPJ_CLRSPC_UNKNOWN;
    }
    w = raw_cp->rawWidth;
    h = raw_cp->rawHeight;
    cmptparm = (opj_image_cmptparm_t*) calloc((OPJ_UINT32)numcomps,
               sizeof(opj_image_cmptparm_t));
    if (!cmptparm) {
        fprintf(stderr, "Failed to allocate image components parameters !!\n");
        fprintf(stderr, "Aborting\n");
        fclose(f);
        return NULL;
    }
    /* initialize image components */
    for (i = 0; i < numcomps; i++) {
        cmptparm[i].prec = (OPJ_UINT32)raw_cp->rawBitDepth;
        cmptparm[i].bpp = (OPJ_UINT32)raw_cp->rawBitDepth;
        cmptparm[i].sgnd = (OPJ_UINT32)raw_cp->rawSigned;
        cmptparm[i].dx = (OPJ_UINT32)(subsampling_dx * raw_cp->rawComps[i].dx);
        cmptparm[i].dy = (OPJ_UINT32)(subsampling_dy * raw_cp->rawComps[i].dy);
        cmptparm[i].w = (OPJ_UINT32)w;
        cmptparm[i].h = (OPJ_UINT32)h;
    }
    /* create the image */
    image = opj_image_create((OPJ_UINT32)numcomps, &cmptparm[0], color_space);
    free(cmptparm);
    if (!image) {
        fclose(f);
        return NULL;
    }
    /* set image offset and reference grid */
    image->x0 = (OPJ_UINT32)parameters->image_offset_x0;
    image->y0 = (OPJ_UINT32)parameters->image_offset_y0;
    image->x1 = (OPJ_UINT32)parameters->image_offset_x0 + (OPJ_UINT32)(w - 1) *
                (OPJ_UINT32)subsampling_dx + 1;
    image->y1 = (OPJ_UINT32)parameters->image_offset_y0 + (OPJ_UINT32)(h - 1) *
                (OPJ_UINT32)subsampling_dy + 1;

    if (raw_cp->rawBitDepth <= 8) {
        unsigned char value = 0;
        for (compno = 0; compno < numcomps; compno++) {
            int nloop = (w * h) / (raw_cp->rawComps[compno].dx *
                                   raw_cp->rawComps[compno].dy);
            for (i = 0; i < nloop; i++) {
                if (!fread(&value, 1, 1, f)) {
                    fprintf(stderr, "Error reading raw file. End of file probably reached.\n");
                    opj_image_destroy(image);
                    fclose(f);
                    return NULL;
                }
                image->comps[compno].data[i] = raw_cp->rawSigned ? (char)value : value;
            }
        }
    } else if (raw_cp->rawBitDepth <= 16) {
        unsigned short value;
        for (compno = 0; compno < numcomps; compno++) {
            int nloop = (w * h) / (raw_cp->rawComps[compno].dx *
                                   raw_cp->rawComps[compno].dy);
            for (i = 0; i < nloop; i++) {
                unsigned char temp1;
                unsigned char temp2;
                if (!fread(&temp1, 1, 1, f)) {
                    fprintf(stderr, "Error reading raw file. End of file probably reached.\n");
                    opj_image_destroy(image);
                    fclose(f);
                    return NULL;
                }
                if (!fread(&temp2, 1, 1, f)) {
                    fprintf(stderr, "Error reading raw file. End of file probably reached.\n");
                    opj_image_destroy(image);
                    fclose(f);
                    return NULL;
                }
                if (big_endian) {
                    value = (unsigned short)((temp1 << 8) + temp2);
                } else {
                    value = (unsigned short)((temp2 << 8) + temp1);
                }
                image->comps[compno].data[i] = raw_cp->rawSigned ? (short)value : value;
            }
        }
    } else {
        fprintf(stderr,
                "OpenJPEG cannot encode raw components with bit depth higher than 16 bits.\n");
        opj_image_destroy(image);
        fclose(f);
        return NULL;
    }

    if (fread(&ch, 1, 1, f)) {
        fprintf(stderr, "Warning. End of raw file not reached... processing anyway\n");
    }
    fclose(f);

    return image;
}
