cmsBool  CMSEXPORT cmsNamedColorInfo(const cmsNAMEDCOLORLIST* NamedColorList, cmsUInt32Number nColor,
                                     char* Name,
                                     char* Prefix,
                                     char* Suffix,
                                     cmsUInt16Number* PCS,
                                     cmsUInt16Number* Colorant)
{
    if (NamedColorList == NULL) return FALSE;

    if (nColor >= cmsNamedColorCount(NamedColorList)) return FALSE;

    if (Name) strcpy(Name, NamedColorList->List[nColor].Name);
    if (Prefix) strcpy(Prefix, NamedColorList->Prefix);
    if (Suffix) strcpy(Suffix, NamedColorList->Suffix);
    if (PCS)
        memmove(PCS, NamedColorList ->List[nColor].PCS, 3*sizeof(cmsUInt16Number));

    if (Colorant)
        memmove(Colorant, NamedColorList ->List[nColor].DeviceColorant,
                                sizeof(cmsUInt16Number) * NamedColorList ->ColorantCount);


    return TRUE;
}
