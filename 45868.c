const wchar_t* _cmsMLUgetWide(const cmsMLU* mlu,
                              cmsUInt32Number *len,
                              cmsUInt16Number LanguageCode, cmsUInt16Number CountryCode,
                              cmsUInt16Number* UsedLanguageCode, cmsUInt16Number* UsedCountryCode)
{
    int i;
    int Best = -1;
    _cmsMLUentry* v;

    if (mlu == NULL) return NULL;

    if (mlu -> AllocatedEntries <= 0) return NULL;

    for (i=0; i < mlu ->UsedEntries; i++) {

        v = mlu ->Entries + i;

        if (v -> Language == LanguageCode) {

            if (Best == -1) Best = i;

            if (v -> Country == CountryCode) {

                if (UsedLanguageCode != NULL) *UsedLanguageCode = v ->Language;
                if (UsedCountryCode  != NULL) *UsedCountryCode = v ->Country;

                if (len != NULL) *len = v ->Len;

                return (wchar_t*) ((cmsUInt8Number*) mlu ->MemPool + v -> StrW);        // Found exact match
            }
        }
    }

    if (Best == -1)
        Best = 0;

    v = mlu ->Entries + Best;

    if (UsedLanguageCode != NULL) *UsedLanguageCode = v ->Language;
    if (UsedCountryCode  != NULL) *UsedCountryCode = v ->Country;

    if (len != NULL) *len   = v ->Len;

    return(wchar_t*) ((cmsUInt8Number*) mlu ->MemPool + v ->StrW);
}
