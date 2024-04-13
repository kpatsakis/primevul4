void EvalNamedColorPCS(const cmsFloat32Number In[], cmsFloat32Number Out[], const cmsStage *mpe)
{
    cmsNAMEDCOLORLIST* NamedColorList = (cmsNAMEDCOLORLIST*) mpe ->Data;
    cmsUInt16Number index = (cmsUInt16Number) _cmsQuickSaturateWord(In[0] * 65535.0);

    if (index >= NamedColorList-> nColors) {
        cmsSignalError(NamedColorList ->ContextID, cmsERROR_RANGE, "Color %d out of range; ignored", index);
    }
    else {

            Out[0] = (cmsFloat32Number) (NamedColorList->List[index].PCS[0] / 65535.0);
            Out[1] = (cmsFloat32Number) (NamedColorList->List[index].PCS[1] / 65535.0);
            Out[2] = (cmsFloat32Number) (NamedColorList->List[index].PCS[2] / 65535.0);
    }
}
