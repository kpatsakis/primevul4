static void convert_32s2u_C1R(const OPJ_INT32* pSrc, OPJ_BYTE* pDst,
                              OPJ_SIZE_T length)
{
    OPJ_SIZE_T i;
    for (i = 0; i < (length & ~(OPJ_SIZE_T)3U); i += 4U) {
        OPJ_UINT32 src0 = (OPJ_UINT32)pSrc[i + 0];
        OPJ_UINT32 src1 = (OPJ_UINT32)pSrc[i + 1];
        OPJ_UINT32 src2 = (OPJ_UINT32)pSrc[i + 2];
        OPJ_UINT32 src3 = (OPJ_UINT32)pSrc[i + 3];

        *pDst++ = (OPJ_BYTE)((src0 << 6) | (src1 << 4) | (src2 << 2) | src3);
    }

    if (length & 3U) {
        OPJ_UINT32 src0 = (OPJ_UINT32)pSrc[i + 0];
        OPJ_UINT32 src1 = 0U;
        OPJ_UINT32 src2 = 0U;
        length = length & 3U;

        if (length > 1U) {
            src1 = (OPJ_UINT32)pSrc[i + 1];
            if (length > 2U) {
                src2 = (OPJ_UINT32)pSrc[i + 2];
            }
        }
        *pDst++ = (OPJ_BYTE)((src0 << 6) | (src1 << 4) | (src2 << 2));
    }
}
