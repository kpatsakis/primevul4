cmsBool AllCurvesAreLinear(cmsStage* mpe)
{
    cmsToneCurve** Curves;
    cmsUInt32Number i, n;

    Curves = _cmsStageGetPtrToCurveSet(mpe);
    if (Curves == NULL) return FALSE;

    n = cmsStageOutputChannels(mpe);

    for (i=0; i < n; i++) {
        if (!cmsIsToneCurveLinear(Curves[i])) return FALSE;
    }

    return TRUE;
}
