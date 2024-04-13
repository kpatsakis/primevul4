cmsBool OptimizeMatrixShaper(cmsPipeline** Lut, cmsUInt32Number Intent, cmsUInt32Number* InputFormat, cmsUInt32Number* OutputFormat, cmsUInt32Number* dwFlags)
{
    cmsStage* Curve1, *Curve2;
    cmsStage* Matrix1, *Matrix2;
    _cmsStageMatrixData* Data1;
    _cmsStageMatrixData* Data2;
    cmsMAT3 res;
    cmsBool IdentityMat;
    cmsPipeline* Dest, *Src;

    if (T_CHANNELS(*InputFormat) != 3 || T_CHANNELS(*OutputFormat) != 3) return FALSE;

    if (!_cmsFormatterIs8bit(*InputFormat)) return FALSE;

    Src = *Lut;

    if (!cmsPipelineCheckAndRetreiveStages(Src, 4,
        cmsSigCurveSetElemType, cmsSigMatrixElemType, cmsSigMatrixElemType, cmsSigCurveSetElemType,
        &Curve1, &Matrix1, &Matrix2, &Curve2)) return FALSE;

    Data1 = (_cmsStageMatrixData*) cmsStageData(Matrix1);
    Data2 = (_cmsStageMatrixData*) cmsStageData(Matrix2);

    if (Data1 ->Offset != NULL) return FALSE;

    _cmsMAT3per(&res, (cmsMAT3*) Data2 ->Double, (cmsMAT3*) Data1 ->Double);

    IdentityMat = FALSE;
    if (_cmsMAT3isIdentity(&res) && Data2 ->Offset == NULL) {

        IdentityMat = TRUE;
    }

    Dest =  cmsPipelineAlloc(Src ->ContextID, Src ->InputChannels, Src ->OutputChannels);
    if (!Dest) return FALSE;

    if (!cmsPipelineInsertStage(Dest, cmsAT_BEGIN, cmsStageDup(Curve1)))
        goto Error;

    if (!IdentityMat)
        if (!cmsPipelineInsertStage(Dest, cmsAT_END, cmsStageAllocMatrix(Dest ->ContextID, 3, 3, (const cmsFloat64Number*) &res, Data2 ->Offset)))
            goto Error;
    if (!cmsPipelineInsertStage(Dest, cmsAT_END, cmsStageDup(Curve2)))
        goto Error;

    if (IdentityMat) {

        OptimizeByJoiningCurves(&Dest, Intent, InputFormat, OutputFormat, dwFlags);
    }
    else {
        _cmsStageToneCurvesData* mpeC1 = (_cmsStageToneCurvesData*) cmsStageData(Curve1);
        _cmsStageToneCurvesData* mpeC2 = (_cmsStageToneCurvesData*) cmsStageData(Curve2);

        *dwFlags |= cmsFLAGS_NOCACHE;

        SetMatShaper(Dest, mpeC1 ->TheCurves, &res, (cmsVEC3*) Data2 ->Offset, mpeC2->TheCurves, OutputFormat);
    }

    cmsPipelineFree(Src);
    *Lut = Dest;
    return TRUE;
Error:
    cmsPipelineFree(Dest);
    return FALSE;
}
