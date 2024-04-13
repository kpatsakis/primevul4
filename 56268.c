static void bmp24toimage(const OPJ_UINT8* pData, OPJ_UINT32 stride,
                         opj_image_t* image)
{
    int index;
    OPJ_UINT32 width, height;
    OPJ_UINT32 x, y;
    const OPJ_UINT8 *pSrc = NULL;

    width  = image->comps[0].w;
    height = image->comps[0].h;

    index = 0;
    pSrc = pData + (height - 1U) * stride;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            image->comps[0].data[index] = (OPJ_INT32)pSrc[3 * x + 2]; /* R */
            image->comps[1].data[index] = (OPJ_INT32)pSrc[3 * x + 1]; /* G */
            image->comps[2].data[index] = (OPJ_INT32)pSrc[3 * x + 0]; /* B */
            index++;
        }
        pSrc -= stride;
    }
}
