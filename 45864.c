void FreeNamedColorList(cmsStage* mpe)
{
    cmsNAMEDCOLORLIST* List = (cmsNAMEDCOLORLIST*) mpe ->Data;
    cmsFreeNamedColorList(List);
}
