cmsBool OptimizeByComputingLinearization(cmsPipeline** Lut, cmsUInt32Number Intent, cmsUInt32Number* InputFormat, cmsUInt32Number* OutputFormat, cmsUInt32Number* dwFlags)
{
    cmsPipeline* OriginalLut;
    int nGridPoints;
    cmsToneCurve *Trans[cmsMAXCHANNELS], *TransReverse[cmsMAXCHANNELS];
    cmsUInt32Number t, i;
    cmsFloat32Number v, In[cmsMAXCHANNELS], Out[cmsMAXCHANNELS];
    cmsBool lIsSuitable, lIsLinear;
    cmsPipeline* OptimizedLUT = NULL, *LutPlusCurves = NULL;
    cmsStage* OptimizedCLUTmpe;
    cmsColorSpaceSignature ColorSpace, OutputColorSpace;
    cmsStage* OptimizedPrelinMpe;
    cmsStage* mpe;
    cmsToneCurve**   OptimizedPrelinCurves;
    _cmsStageCLutData*     OptimizedPrelinCLUT;


    if (_cmsFormatterIsFloat(*InputFormat) || _cmsFormatterIsFloat(*OutputFormat)) return FALSE;

    if (T_COLORSPACE(*InputFormat)  != PT_RGB) return FALSE;
    if (T_COLORSPACE(*OutputFormat) != PT_RGB) return FALSE;


    if (!_cmsFormatterIs8bit(*InputFormat)) {
        if (!(*dwFlags & cmsFLAGS_CLUT_PRE_LINEARIZATION)) return FALSE;
    }

    OriginalLut = *Lut;

   for (mpe = cmsPipelineGetPtrToFirstStage(OriginalLut);
         mpe != NULL;
         mpe = cmsStageNext(mpe)) {
            if (cmsStageType(mpe) == cmsSigNamedColorElemType) return FALSE;
    }

    ColorSpace       = _cmsICCcolorSpace(T_COLORSPACE(*InputFormat));
    OutputColorSpace = _cmsICCcolorSpace(T_COLORSPACE(*OutputFormat));
    nGridPoints      = _cmsReasonableGridpointsByColorspace(ColorSpace, *dwFlags);

    memset(Trans, 0, sizeof(Trans));
    memset(TransReverse, 0, sizeof(TransReverse));

    for (t = 0; t < OriginalLut ->InputChannels; t++) {
        Trans[t] = cmsBuildTabulatedToneCurve16(OriginalLut ->ContextID, PRELINEARIZATION_POINTS, NULL);
        if (Trans[t] == NULL) goto Error;
    }

    for (i=0; i < PRELINEARIZATION_POINTS; i++) {

        v = (cmsFloat32Number) ((cmsFloat64Number) i / (PRELINEARIZATION_POINTS - 1));

        for (t=0; t < OriginalLut ->InputChannels; t++)
            In[t] = v;

        cmsPipelineEvalFloat(In, Out, OriginalLut);

        for (t=0; t < OriginalLut ->InputChannels; t++)
            Trans[t] ->Table16[i] = _cmsQuickSaturateWord(Out[t] * 65535.0);
    }

    for (t = 0; t < OriginalLut ->InputChannels; t++)
        SlopeLimiting(Trans[t]);

    lIsSuitable = TRUE;
    lIsLinear   = TRUE;
    for (t=0; (lIsSuitable && (t < OriginalLut ->InputChannels)); t++) {

        if (!cmsIsToneCurveLinear(Trans[t]))
            lIsLinear = FALSE;

        if (!cmsIsToneCurveMonotonic(Trans[t]))
            lIsSuitable = FALSE;

        if (IsDegenerated(Trans[t]))
            lIsSuitable = FALSE;
    }

    if (!lIsSuitable) goto Error;

    for (t = 0; t < OriginalLut ->InputChannels; t++) {
        TransReverse[t] = cmsReverseToneCurveEx(PRELINEARIZATION_POINTS, Trans[t]);
        if (TransReverse[t] == NULL) goto Error;
    }

    LutPlusCurves = cmsPipelineDup(OriginalLut);
    if (LutPlusCurves == NULL) goto Error;

    if (!cmsPipelineInsertStage(LutPlusCurves, cmsAT_BEGIN, cmsStageAllocToneCurves(OriginalLut ->ContextID, OriginalLut ->InputChannels, TransReverse)))
        goto Error;

    OptimizedLUT = cmsPipelineAlloc(OriginalLut ->ContextID, OriginalLut ->InputChannels, OriginalLut ->OutputChannels);
    if (OptimizedLUT == NULL) goto Error;

    OptimizedPrelinMpe = cmsStageAllocToneCurves(OriginalLut ->ContextID, OriginalLut ->InputChannels, Trans);

    if (!cmsPipelineInsertStage(OptimizedLUT, cmsAT_BEGIN, OptimizedPrelinMpe))
        goto Error;

    OptimizedCLUTmpe = cmsStageAllocCLut16bit(OriginalLut ->ContextID, nGridPoints, OriginalLut ->InputChannels, OriginalLut ->OutputChannels, NULL);

    if (!cmsPipelineInsertStage(OptimizedLUT, cmsAT_END, OptimizedCLUTmpe))
        goto Error;

    if (!cmsStageSampleCLut16bit(OptimizedCLUTmpe, XFormSampler16, (void*) LutPlusCurves, 0)) goto Error;

    for (t = 0; t < OriginalLut ->InputChannels; t++) {

        if (Trans[t]) cmsFreeToneCurve(Trans[t]);
        if (TransReverse[t]) cmsFreeToneCurve(TransReverse[t]);
    }

    cmsPipelineFree(LutPlusCurves);


    OptimizedPrelinCurves = _cmsStageGetPtrToCurveSet(OptimizedPrelinMpe);
    OptimizedPrelinCLUT   = (_cmsStageCLutData*) OptimizedCLUTmpe ->Data;

    if (_cmsFormatterIs8bit(*InputFormat)) {

        Prelin8Data* p8 = PrelinOpt8alloc(OptimizedLUT ->ContextID,
                                                OptimizedPrelinCLUT ->Params,
                                                OptimizedPrelinCurves);
        if (p8 == NULL) return FALSE;

        _cmsPipelineSetOptimizationParameters(OptimizedLUT, PrelinEval8, (void*) p8, Prelin8free, Prelin8dup);

    }
    else
    {
        Prelin16Data* p16 = PrelinOpt16alloc(OptimizedLUT ->ContextID,
            OptimizedPrelinCLUT ->Params,
            3, OptimizedPrelinCurves, 3, NULL);
        if (p16 == NULL) return FALSE;

        _cmsPipelineSetOptimizationParameters(OptimizedLUT, PrelinEval16, (void*) p16, PrelinOpt16free, Prelin16dup);

    }

    if (Intent == INTENT_ABSOLUTE_COLORIMETRIC)
        *dwFlags |= cmsFLAGS_NOWHITEONWHITEFIXUP;

    if (!(*dwFlags & cmsFLAGS_NOWHITEONWHITEFIXUP)) {

        if (!FixWhiteMisalignment(OptimizedLUT, ColorSpace, OutputColorSpace)) {

            return FALSE;
        }
    }


    cmsPipelineFree(OriginalLut);
    *Lut = OptimizedLUT;
    return TRUE;

Error:

    for (t = 0; t < OriginalLut ->InputChannels; t++) {

        if (Trans[t]) cmsFreeToneCurve(Trans[t]);
        if (TransReverse[t]) cmsFreeToneCurve(TransReverse[t]);
    }

    if (LutPlusCurves != NULL) cmsPipelineFree(LutPlusCurves);
    if (OptimizedLUT != NULL) cmsPipelineFree(OptimizedLUT);

    return FALSE;

    cmsUNUSED_PARAMETER(Intent);
}
