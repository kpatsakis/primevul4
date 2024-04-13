static void bmpmask16toimage(const OPJ_UINT8* pData, OPJ_UINT32 stride,
                             opj_image_t* image, OPJ_UINT32 redMask, OPJ_UINT32 greenMask,
                             OPJ_UINT32 blueMask, OPJ_UINT32 alphaMask)
{
    int index;
    OPJ_UINT32 width, height;
    OPJ_UINT32 x, y;
    const OPJ_UINT8 *pSrc = NULL;
    OPJ_BOOL hasAlpha;
    OPJ_UINT32 redShift,   redPrec;
    OPJ_UINT32 greenShift, greenPrec;
    OPJ_UINT32 blueShift,  bluePrec;
    OPJ_UINT32 alphaShift, alphaPrec;

    width  = image->comps[0].w;
    height = image->comps[0].h;

    hasAlpha = image->numcomps > 3U;

    bmp_mask_get_shift_and_prec(redMask,   &redShift,   &redPrec);
    bmp_mask_get_shift_and_prec(greenMask, &greenShift, &greenPrec);
    bmp_mask_get_shift_and_prec(blueMask,  &blueShift,  &bluePrec);
    bmp_mask_get_shift_and_prec(alphaMask, &alphaShift, &alphaPrec);

    image->comps[0].bpp = redPrec;
    image->comps[0].prec = redPrec;
    image->comps[1].bpp = greenPrec;
    image->comps[1].prec = greenPrec;
    image->comps[2].bpp = bluePrec;
    image->comps[2].prec = bluePrec;
    if (hasAlpha) {
        image->comps[3].bpp = alphaPrec;
        image->comps[3].prec = alphaPrec;
    }

    index = 0;
    pSrc = pData + (height - 1U) * stride;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            OPJ_UINT32 value = 0U;

            value |= ((OPJ_UINT32)pSrc[2 * x + 0]) <<  0;
            value |= ((OPJ_UINT32)pSrc[2 * x + 1]) <<  8;

            image->comps[0].data[index] = (OPJ_INT32)((value & redMask)   >>
                                          redShift);   /* R */
            image->comps[1].data[index] = (OPJ_INT32)((value & greenMask) >>
                                          greenShift); /* G */
            image->comps[2].data[index] = (OPJ_INT32)((value & blueMask)  >>
                                          blueShift);  /* B */
            if (hasAlpha) {
                image->comps[3].data[index] = (OPJ_INT32)((value & alphaMask)  >>
                                              alphaShift);  /* A */
            }
            index++;
        }
        pSrc -= stride;
    }
}
