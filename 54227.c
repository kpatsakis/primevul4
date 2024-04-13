int BlackPreservingSampler(register const cmsUInt16Number In[], register cmsUInt16Number Out[], register void* Cargo)
{
    int i;
    cmsFloat32Number Inf[4], Outf[4];
    cmsFloat32Number LabK[4];
    cmsFloat64Number SumCMY, SumCMYK, Error, Ratio;
    cmsCIELab ColorimetricLab, BlackPreservingLab;
    PreserveKPlaneParams* bp = (PreserveKPlaneParams*) Cargo;

    for (i=0; i < 4; i++)
        Inf[i] = (cmsFloat32Number) (In[i] / 65535.0);

    LabK[3] = cmsEvalToneCurveFloat(bp ->KTone, Inf[3]);

    if (In[0] == 0 && In[1] == 0 && In[2] == 0) {

        Out[0] = Out[1] = Out[2] = 0;
        Out[3] = _cmsQuickSaturateWord(LabK[3] * 65535.0);
        return TRUE;
    }

    cmsPipelineEvalFloat( Inf, Outf, bp ->cmyk2cmyk);

    for (i=0; i < 4; i++)
            Out[i] = _cmsQuickSaturateWord(Outf[i] * 65535.0);

    if ( fabs(Outf[3] - LabK[3]) < (3.0 / 65535.0) ) {
        return TRUE;
    }

    cmsDoTransform(bp->hProofOutput, Out, &ColorimetricLab, 1);

    cmsDoTransform(bp ->cmyk2Lab, Outf, LabK, 1);

    if (!cmsPipelineEvalReverseFloat(LabK, Outf, Outf, bp ->LabK2cmyk)) {

        return TRUE;
    }

    Outf[3] = LabK[3];

    SumCMY   = Outf[0]  + Outf[1] + Outf[2];
    SumCMYK  = SumCMY + Outf[3];

    if (SumCMYK > bp ->MaxTAC) {

        Ratio = 1 - ((SumCMYK - bp->MaxTAC) / SumCMY);
        if (Ratio < 0)
            Ratio = 0;
    }
    else
       Ratio = 1.0;

    Out[0] = _cmsQuickSaturateWord(Outf[0] * Ratio * 65535.0);     // C
    Out[1] = _cmsQuickSaturateWord(Outf[1] * Ratio * 65535.0);     // M
    Out[2] = _cmsQuickSaturateWord(Outf[2] * Ratio * 65535.0);     // Y
    Out[3] = _cmsQuickSaturateWord(Outf[3] * 65535.0);

    cmsDoTransform(bp->hProofOutput, Out, &BlackPreservingLab, 1);
    Error = cmsDeltaE(&ColorimetricLab, &BlackPreservingLab);
    if (Error > bp -> MaxError)
        bp->MaxError = Error;

    return TRUE;
}
