void* DupNamedColorList(cmsStage* mpe)
{
    cmsNAMEDCOLORLIST* List = (cmsNAMEDCOLORLIST*) mpe ->Data;
    return cmsDupNamedColorList(List);
}
