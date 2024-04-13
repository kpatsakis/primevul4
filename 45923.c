cmsBool WhitesAreEqual(int n, cmsUInt16Number White1[], cmsUInt16Number White2[] )
{
    int i;

    for (i=0; i < n; i++) {

        if (abs(White1[i] - White2[i]) > 0xf000) return TRUE;  // Values are so extremly different that the fixup should be avoided
        if (White1[i] != White2[i]) return FALSE;
    }
    return TRUE;
}
