cmsBool CMSEXPORT cmsMLUtranslationsCodes(const cmsMLU* mlu,
                                          cmsUInt32Number idx,
                                          char LanguageCode[3],
                                          char CountryCode[3])
{
    _cmsMLUentry *entry;

    if (mlu == NULL) return FALSE;

    if (idx >= (cmsUInt32Number) mlu->UsedEntries) return FALSE;

    entry = &mlu->Entries[idx];
    
    *(cmsUInt16Number *)LanguageCode = _cmsAdjustEndianess16(entry->Language);
    *(cmsUInt16Number *)CountryCode  = _cmsAdjustEndianess16(entry->Country);

    return TRUE;
}
