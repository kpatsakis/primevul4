cmsUInt32Number CMSEXPORT cmsMLUgetASCII(const cmsMLU* mlu,
                                       const char LanguageCode[3], const char CountryCode[3],
                                       char* Buffer, cmsUInt32Number BufferSize)
{
    const wchar_t *Wide;
    cmsUInt32Number  StrLen = 0;
    cmsUInt32Number ASCIIlen, i;

    cmsUInt16Number Lang  = _cmsAdjustEndianess16(*(cmsUInt16Number*) LanguageCode);
    cmsUInt16Number Cntry = _cmsAdjustEndianess16(*(cmsUInt16Number*) CountryCode);

    if (mlu == NULL) return 0;

    Wide = _cmsMLUgetWide(mlu, &StrLen, Lang, Cntry, NULL, NULL);
    if (Wide == NULL) return 0;

    ASCIIlen = StrLen / sizeof(wchar_t);

    if (Buffer == NULL) return ASCIIlen + 1; // Note the zero at the end

    if (BufferSize <= 0) return 0;

    if (BufferSize < ASCIIlen + 1)
        ASCIIlen = BufferSize - 1;

    for (i=0; i < ASCIIlen; i++) {

        if (Wide[i] == 0)
            Buffer[i] = 0;
        else
            Buffer[i] = (char) Wide[i];
    }

    Buffer[ASCIIlen] = 0;
    return ASCIIlen + 1;
}
