const cmsDICTentry* CMSEXPORT cmsDictGetEntryList(cmsHANDLE hDict)
{
    _cmsDICT* dict = (_cmsDICT*) hDict;

    if (dict == NULL) return NULL;
    return dict ->head;
}
