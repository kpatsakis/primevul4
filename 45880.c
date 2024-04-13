cmsNAMEDCOLORLIST* CMSEXPORT cmsGetNamedColorList(cmsHTRANSFORM xform)
{
    _cmsTRANSFORM* v = (_cmsTRANSFORM*) xform;
    cmsStage* mpe  = v ->Lut->Elements;

    if (mpe ->Type != cmsSigNamedColorElemType) return NULL;
    return (cmsNAMEDCOLORLIST*) mpe ->Data;
}
