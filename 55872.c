static void convert_32s_P3C3(OPJ_INT32 const* const* pSrc, OPJ_INT32* pDst,
                             OPJ_SIZE_T length, OPJ_INT32 adjust)
{
    OPJ_SIZE_T i;
    const OPJ_INT32* pSrc0 = pSrc[0];
    const OPJ_INT32* pSrc1 = pSrc[1];
    const OPJ_INT32* pSrc2 = pSrc[2];

    for (i = 0; i < length; i++) {
        pDst[3 * i + 0] = pSrc0[i] + adjust;
        pDst[3 * i + 1] = pSrc1[i] + adjust;
        pDst[3 * i + 2] = pSrc2[i] + adjust;
    }
}
