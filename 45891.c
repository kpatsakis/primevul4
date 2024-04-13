cmsInt32Number CMSEXPORT cmsNamedColorIndex(const cmsNAMEDCOLORLIST* NamedColorList, const char* Name)
{
    int i, n;

    if (NamedColorList == NULL) return -1;
    n = cmsNamedColorCount(NamedColorList);
    for (i=0; i < n; i++) {
        if (cmsstrcasecmp(Name,  NamedColorList->List[i].Name) == 0)
            return i;
    }

    return -1;
}
