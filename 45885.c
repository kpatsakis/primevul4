cmsUInt32Number CMSEXPORT cmsMLUgetWide(const cmsMLU* mlu,
                                      const char LanguageCode[3], const char CountryCode[3],
                                      wchar_t* Buffer, cmsUInt32Number BufferSize)
{
    const wchar_t *Wide;
    cmsUInt32Number  StrLen = 0;

    cmsUInt16Number Lang  = _cmsAdjustEndianess16(*(cmsUInt16Number*) LanguageCode);
    cmsUInt16Number Cntry = _cmsAdjustEndianess16(*(cmsUInt16Number*) CountryCode);

    if (mlu == NULL) return 0;

    Wide = _cmsMLUgetWide(mlu, &StrLen, Lang, Cntry, NULL, NULL);
    if (Wide == NULL) return 0;

    if (Buffer == NULL) return StrLen + sizeof(wchar_t);

    if (BufferSize <= 0) return 0;

    if (BufferSize < StrLen + sizeof(wchar_t))
        StrLen = BufferSize - + sizeof(wchar_t);

    memmove(Buffer, Wide, StrLen);
    Buffer[StrLen / sizeof(wchar_t)] = 0;

    return StrLen + sizeof(wchar_t);
}
