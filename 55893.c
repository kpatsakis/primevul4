static INLINE OPJ_UINT16 swap16(OPJ_UINT16 x)
{
    return (OPJ_UINT16)(((x & 0x00ffU) <<  8) | ((x & 0xff00U) >>  8));
}
