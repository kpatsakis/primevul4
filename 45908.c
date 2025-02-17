cmsBool OptimizeByJoiningCurves(cmsPipeline** Lut, cmsUInt32Number Intent, cmsUInt32Number* InputFormat, cmsUInt32Number* OutputFormat, cmsUInt32Number* dwFlags)
{
    cmsToneCurve** GammaTables = NULL;
    cmsFloat32Number InFloat[cmsMAXCHANNELS], OutFloat[cmsMAXCHANNELS];
    cmsUInt32Number i, j;
    cmsPipeline* Src = *Lut;
    cmsPipeline* Dest = NULL;
    cmsStage* mpe;
    cmsStage* ObtainedCurves = NULL;


    if (_cmsFormatterIsFloat(*InputFormat) || _cmsFormatterIsFloat(*OutputFormat)) return FALSE;

    for (mpe = cmsPipelineGetPtrToFirstStage(Src);
         mpe != NULL;
         mpe = cmsStageNext(mpe)) {
            if (cmsStageType(mpe) != cmsSigCurveSetElemType) return FALSE;
    }

    Dest =  cmsPipelineAlloc(Src ->ContextID, Src ->InputChannels, Src ->OutputChannels);
    if (Dest == NULL) return FALSE;

    GammaTables = (cmsToneCurve**) _cmsCalloc(Src ->ContextID, Src ->InputChannels, sizeof(cmsToneCurve*));
    if (GammaTables == NULL) goto Error;

    for (i=0; i < Src ->InputChannels; i++) {
        GammaTables[i] = cmsBuildTabulatedToneCurve16(Src ->ContextID, PRELINEARIZATION_POINTS, NULL);
        if (GammaTables[i] == NULL) goto Error;
    }

    for (i=0; i < PRELINEARIZATION_POINTS; i++) {

        for (j=0; j < Src ->InputChannels; j++)
            InFloat[j] = (cmsFloat32Number) ((cmsFloat64Number) i / (PRELINEARIZATION_POINTS - 1));

        cmsPipelineEvalFloat(InFloat, OutFloat, Src);

        for (j=0; j < Src ->InputChannels; j++)
            GammaTables[j] -> Table16[i] = _cmsQuickSaturateWord(OutFloat[j] * 65535.0);
    }

    ObtainedCurves = cmsStageAllocToneCurves(Src ->ContextID, Src ->InputChannels, GammaTables);
    if (ObtainedCurves == NULL) goto Error;

    for (i=0; i < Src ->InputChannels; i++) {
        cmsFreeToneCurve(GammaTables[i]);
        GammaTables[i] = NULL;
    }

    if (GammaTables != NULL) _cmsFree(Src ->ContextID, GammaTables);

    if (!AllCurvesAreLinear(ObtainedCurves)) {

        if (!cmsPipelineInsertStage(Dest, cmsAT_BEGIN, ObtainedCurves))
            goto Error;

        if (_cmsFormatterIs8bit(*InputFormat)) {

            _cmsStageToneCurvesData* Data = (_cmsStageToneCurvesData*) ObtainedCurves ->Data;
             Curves16Data* c16 = CurvesAlloc(Dest ->ContextID, Data ->nCurves, 256, Data ->TheCurves);

             if (c16 == NULL) goto Error; 
             *dwFlags |= cmsFLAGS_NOCACHE;
            _cmsPipelineSetOptimizationParameters(Dest, FastEvaluateCurves8, c16, CurvesFree, CurvesDup);

        }
        else {

            _cmsStageToneCurvesData* Data = (_cmsStageToneCurvesData*) cmsStageData(ObtainedCurves);
             Curves16Data* c16 = CurvesAlloc(Dest ->ContextID, Data ->nCurves, 65536, Data ->TheCurves);

             if (c16 == NULL) goto Error; 
             *dwFlags |= cmsFLAGS_NOCACHE;
            _cmsPipelineSetOptimizationParameters(Dest, FastEvaluateCurves16, c16, CurvesFree, CurvesDup);
        }
    }
    else {

        cmsStageFree(ObtainedCurves);

        if (!cmsPipelineInsertStage(Dest, cmsAT_BEGIN, cmsStageAllocIdentity(Dest ->ContextID, Src ->InputChannels)))
            goto Error;

        *dwFlags |= cmsFLAGS_NOCACHE;
        _cmsPipelineSetOptimizationParameters(Dest, FastIdentity16, (void*) Dest, NULL, NULL);
    }

    cmsPipelineFree(Src);
    *Lut = Dest;
    return TRUE;

Error:

    if (ObtainedCurves != NULL) cmsStageFree(ObtainedCurves);
    if (GammaTables != NULL) {
        for (i=0; i < Src ->InputChannels; i++) {
            if (GammaTables[i] != NULL) cmsFreeToneCurve(GammaTables[i]);
        }

        _cmsFree(Src ->ContextID, GammaTables);
    }

    if (Dest != NULL) cmsPipelineFree(Dest);
    return FALSE;

    cmsUNUSED_PARAMETER(Intent);
    cmsUNUSED_PARAMETER(InputFormat);
    cmsUNUSED_PARAMETER(OutputFormat);
    cmsUNUSED_PARAMETER(dwFlags);
}
