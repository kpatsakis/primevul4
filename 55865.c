static void convert_32s8u_C1R(const OPJ_INT32* pSrc, OPJ_BYTE* pDst,
                              OPJ_SIZE_T length)
{
    OPJ_SIZE_T i;
    for (i = 0; i < length; ++i) {
        pDst[i] = (OPJ_BYTE)pSrc[i];
    }
}
