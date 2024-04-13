cmsHANDLE CMSEXPORT cmsDictDup(cmsHANDLE hDict)
{
    _cmsDICT* old_dict = (_cmsDICT*) hDict;
    cmsHANDLE hNew;
    cmsDICTentry *entry;

    _cmsAssert(old_dict != NULL);

    hNew  = cmsDictAlloc(old_dict ->ContextID);
    if (hNew == NULL) return NULL;

    entry = old_dict ->head;
    while (entry != NULL) {

        if (!cmsDictAddEntry(hNew, entry ->Name, entry ->Value, entry ->DisplayName, entry ->DisplayValue)) {

            cmsDictFree(hNew);
            return NULL;
        }

        entry = entry -> Next;
    }

    return hNew;
}
