cmsBool ColorSpaceIsCompatible(cmsColorSpaceSignature a, cmsColorSpaceSignature b)
{
    if (a == b) return TRUE;

    if ((a == cmsSig4colorData) && (b == cmsSigCmykData)) return TRUE;
    if ((a == cmsSigCmykData) && (b == cmsSig4colorData)) return TRUE;

    if ((a == cmsSigXYZData) && (b == cmsSigLabData)) return TRUE;
    if ((a == cmsSigLabData) && (b == cmsSigXYZData)) return TRUE;

    return FALSE;
}
