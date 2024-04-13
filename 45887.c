cmsBool  CMSEXPORT cmsMLUsetWide(cmsMLU* mlu, const char Language[3], const char Country[3], const wchar_t* WideString)
{
    cmsUInt16Number Lang  = _cmsAdjustEndianess16(*(cmsUInt16Number*) Language);
    cmsUInt16Number Cntry = _cmsAdjustEndianess16(*(cmsUInt16Number*) Country);
    cmsUInt32Number len;

    if (mlu == NULL) return FALSE;
    if (WideString == NULL) return FALSE;

    len = (cmsUInt32Number) (mywcslen(WideString) + 1) * sizeof(wchar_t);
    return AddMLUBlock(mlu, len, WideString, Lang, Cntry);
}
