void CMSEXPORT cmsDictFree(cmsHANDLE hDict)
{
    _cmsDICT* dict = (_cmsDICT*) hDict;
    cmsDICTentry *entry, *next;

    _cmsAssert(dict != NULL);

    entry = dict ->head;
    while (entry != NULL) {

            if (entry ->DisplayName  != NULL) cmsMLUfree(entry ->DisplayName);
            if (entry ->DisplayValue != NULL) cmsMLUfree(entry ->DisplayValue);
            if (entry ->Name != NULL) _cmsFree(dict ->ContextID, entry -> Name);
            if (entry ->Value != NULL) _cmsFree(dict ->ContextID, entry -> Value);

            next = entry ->Next;
            _cmsFree(dict ->ContextID, entry);

            entry = next;
    }

    _cmsFree(dict ->ContextID, dict);
}
