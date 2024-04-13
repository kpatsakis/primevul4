cmsHANDLE CMSEXPORT cmsDictAlloc(cmsContext ContextID)
{
    _cmsDICT* dict = (_cmsDICT*) _cmsMallocZero(ContextID, sizeof(_cmsDICT));
    if (dict == NULL) return NULL;

    dict ->ContextID = ContextID;
    return (cmsHANDLE) dict;

}
