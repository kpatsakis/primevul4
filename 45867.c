cmsBool  GrowNamedColorList(cmsNAMEDCOLORLIST* v)
{
    cmsUInt32Number size;
    _cmsNAMEDCOLOR * NewPtr;

    if (v == NULL) return FALSE;

    if (v ->Allocated == 0)
        size = 64;   // Initial guess
    else
        size = v ->Allocated * 2;

    if (size > 1024*100) return FALSE;

    NewPtr = (_cmsNAMEDCOLOR*) _cmsRealloc(v ->ContextID, v ->List, size * sizeof(_cmsNAMEDCOLOR));
    if (NewPtr == NULL)
        return FALSE;

    v ->List      = NewPtr;
    v ->Allocated = size;
    return TRUE;
}
