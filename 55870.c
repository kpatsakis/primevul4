static void convert_32s_P1C1(OPJ_INT32 const* const* pSrc, OPJ_INT32* pDst,
                             OPJ_SIZE_T length, OPJ_INT32 adjust)
{
    OPJ_SIZE_T i;
    const OPJ_INT32* pSrc0 = pSrc[0];

    for (i = 0; i < length; i++) {
        pDst[i] = pSrc0[i] + adjust;
    }
}
