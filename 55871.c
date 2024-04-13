static void convert_32s_P2C2(OPJ_INT32 const* const* pSrc, OPJ_INT32* pDst,
                             OPJ_SIZE_T length, OPJ_INT32 adjust)
{
    OPJ_SIZE_T i;
    const OPJ_INT32* pSrc0 = pSrc[0];
    const OPJ_INT32* pSrc1 = pSrc[1];

    for (i = 0; i < length; i++) {
        pDst[2 * i + 0] = pSrc0[i] + adjust;
        pDst[2 * i + 1] = pSrc1[i] + adjust;
    }
}
