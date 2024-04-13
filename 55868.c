static void convert_32s_C3P3(const OPJ_INT32* pSrc, OPJ_INT32* const* pDst,
                             OPJ_SIZE_T length)
{
    OPJ_SIZE_T i;
    OPJ_INT32* pDst0 = pDst[0];
    OPJ_INT32* pDst1 = pDst[1];
    OPJ_INT32* pDst2 = pDst[2];

    for (i = 0; i < length; i++) {
        pDst0[i] = pSrc[3 * i + 0];
        pDst1[i] = pSrc[3 * i + 1];
        pDst2[i] = pSrc[3 * i + 2];
    }
}
