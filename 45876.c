cmsNAMEDCOLORLIST* CMSEXPORT cmsDupNamedColorList(const cmsNAMEDCOLORLIST* v)
{
    cmsNAMEDCOLORLIST* NewNC;

    if (v == NULL) return NULL;

    NewNC= cmsAllocNamedColorList(v ->ContextID, v -> nColors, v ->ColorantCount, v ->Prefix, v ->Suffix);
    if (NewNC == NULL) return NULL;

    while (NewNC ->Allocated < v ->Allocated)
        GrowNamedColorList(NewNC);

    memmove(NewNC ->Prefix, v ->Prefix, sizeof(v ->Prefix));
    memmove(NewNC ->Suffix, v ->Suffix, sizeof(v ->Suffix));
    NewNC ->ColorantCount = v ->ColorantCount;
    memmove(NewNC->List, v ->List, v->nColors * sizeof(_cmsNAMEDCOLOR));
    NewNC ->nColors = v ->nColors;
    return NewNC;
}
