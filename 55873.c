static void convert_4u32s_C1R(const OPJ_BYTE* pSrc, OPJ_INT32* pDst,
                              OPJ_SIZE_T length)
{
    OPJ_SIZE_T i;
    for (i = 0; i < (length & ~(OPJ_SIZE_T)1U); i += 2U) {
        OPJ_UINT32 val = *pSrc++;
        pDst[i + 0] = (OPJ_INT32)(val >> 4);
        pDst[i + 1] = (OPJ_INT32)(val & 0xFU);
    }
    if (length & 1U) {
        OPJ_UINT8 val = *pSrc++;
        pDst[i + 0] = (OPJ_INT32)(val >> 4);
    }
}
