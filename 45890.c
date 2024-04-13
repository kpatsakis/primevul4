cmsUInt32Number CMSEXPORT cmsNamedColorCount(const cmsNAMEDCOLORLIST* NamedColorList)
{
     if (NamedColorList == NULL) return 0;
     return NamedColorList ->nColors;
}
