cmsBool CMSEXPORT cmsMLUsetASCII(cmsMLU* mlu, const char LanguageCode[3], const char CountryCode[3], const char* ASCIIString)
{
    cmsUInt32Number i, len = (cmsUInt32Number) strlen(ASCIIString)+1;
    wchar_t* WStr;
    cmsBool  rc;
    cmsUInt16Number Lang  = _cmsAdjustEndianess16(*(cmsUInt16Number*) LanguageCode);
    cmsUInt16Number Cntry = _cmsAdjustEndianess16(*(cmsUInt16Number*) CountryCode);

    if (mlu == NULL) return FALSE;

    WStr = (wchar_t*) _cmsCalloc(mlu ->ContextID, len,  sizeof(wchar_t));
    if (WStr == NULL) return FALSE;

    for (i=0; i < len; i++)
        WStr[i] = (wchar_t) ASCIIString[i];

    rc = AddMLUBlock(mlu, len  * sizeof(wchar_t), WStr, Lang, Cntry);

    _cmsFree(mlu ->ContextID, WStr);
    return rc;

}
