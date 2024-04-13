static void convert_32s_C1P1(const OPJ_INT32* pSrc, OPJ_INT32* const* pDst,
                             OPJ_SIZE_T length)
{
    memcpy(pDst[0], pSrc, length * sizeof(OPJ_INT32));
}
