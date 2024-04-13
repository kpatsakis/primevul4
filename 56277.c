static void opj_applyLUT8u_8u32s_C1R(
    OPJ_UINT8 const* pSrc, OPJ_INT32 srcStride,
    OPJ_INT32* pDst, OPJ_INT32 dstStride,
    OPJ_UINT8 const* pLUT,
    OPJ_UINT32 width, OPJ_UINT32 height)
{
    OPJ_UINT32 y;

    for (y = height; y != 0U; --y) {
        OPJ_UINT32 x;

        for (x = 0; x < width; x++) {
            pDst[x] = (OPJ_INT32)pLUT[pSrc[x]];
        }
        pSrc += srcStride;
        pDst += dstStride;
    }
}
