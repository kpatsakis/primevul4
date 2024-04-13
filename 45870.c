cmsBool CMSEXPORT cmsDictAddEntry(cmsHANDLE hDict, const wchar_t* Name, const wchar_t* Value, const cmsMLU *DisplayName, const cmsMLU *DisplayValue)
{
    _cmsDICT* dict = (_cmsDICT*) hDict;
    cmsDICTentry *entry;

    _cmsAssert(dict != NULL);
    _cmsAssert(Name != NULL);

    entry = (cmsDICTentry*) _cmsMallocZero(dict ->ContextID, sizeof(cmsDICTentry));
    if (entry == NULL) return FALSE;

    entry ->DisplayName  = cmsMLUdup(DisplayName);
    entry ->DisplayValue = cmsMLUdup(DisplayValue);
    entry ->Name         = DupWcs(dict ->ContextID, Name);
    entry ->Value        = DupWcs(dict ->ContextID, Value);

    entry ->Next = dict ->head;
    dict ->head = entry;

    return TRUE;
}
