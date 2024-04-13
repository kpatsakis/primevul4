void EvalNamedColor(const cmsFloat32Number In[], cmsFloat32Number Out[], const cmsStage *mpe)
{
    cmsNAMEDCOLORLIST* NamedColorList = (cmsNAMEDCOLORLIST*) mpe ->Data;
    cmsUInt16Number index = (cmsUInt16Number) _cmsQuickSaturateWord(In[0] * 65535.0);
    cmsUInt32Number j;

    if (index >= NamedColorList-> nColors) {
        cmsSignalError(NamedColorList ->ContextID, cmsERROR_RANGE, "Color %d out of range; ignored", index);
    }
    else {
        for (j=0; j < NamedColorList ->ColorantCount; j++)
            Out[j] = (cmsFloat32Number) (NamedColorList->List[index].DeviceColorant[j] / 65535.0);
    }
}
