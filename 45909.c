cmsBool OptimizeByResampling(cmsPipeline** Lut, cmsUInt32Number Intent, cmsUInt32Number* InputFormat, cmsUInt32Number* OutputFormat, cmsUInt32Number* dwFlags)
{
    cmsPipeline* Src = NULL;
    cmsPipeline* Dest = NULL;
    cmsStage* mpe;
    cmsStage* CLUT;
    cmsStage *KeepPreLin = NULL, *KeepPostLin = NULL;
    int nGridPoints;
    cmsColorSpaceSignature ColorSpace, OutputColorSpace;
    cmsStage *NewPreLin = NULL;
    cmsStage *NewPostLin = NULL;
    _cmsStageCLutData* DataCLUT;
    cmsToneCurve** DataSetIn;
    cmsToneCurve** DataSetOut;
    Prelin16Data* p16;

    if (_cmsFormatterIsFloat(*InputFormat) || _cmsFormatterIsFloat(*OutputFormat)) return FALSE;

    ColorSpace       = _cmsICCcolorSpace(T_COLORSPACE(*InputFormat));
    OutputColorSpace = _cmsICCcolorSpace(T_COLORSPACE(*OutputFormat));
    nGridPoints      = _cmsReasonableGridpointsByColorspace(ColorSpace, *dwFlags);

    if (cmsPipelineStageCount(*Lut) == 0)
        nGridPoints = 2;

    Src = *Lut;

    for (mpe = cmsPipelineGetPtrToFirstStage(Src);
        mpe != NULL;
        mpe = cmsStageNext(mpe)) {
            if (cmsStageType(mpe) == cmsSigNamedColorElemType) return FALSE;
    }

    Dest =  cmsPipelineAlloc(Src ->ContextID, Src ->InputChannels, Src ->OutputChannels);
    if (!Dest) return FALSE;

    if (*dwFlags & cmsFLAGS_CLUT_PRE_LINEARIZATION) {

        cmsStage* PreLin = cmsPipelineGetPtrToFirstStage(Src);

        if (PreLin ->Type == cmsSigCurveSetElemType) {

            if (!AllCurvesAreLinear(PreLin)) {

                NewPreLin = cmsStageDup(PreLin);
                if(!cmsPipelineInsertStage(Dest, cmsAT_BEGIN, NewPreLin))
                    goto Error;

                cmsPipelineUnlinkStage(Src, cmsAT_BEGIN, &KeepPreLin);
            }
        }
    }

    CLUT = cmsStageAllocCLut16bit(Src ->ContextID, nGridPoints, Src ->InputChannels, Src->OutputChannels, NULL);
    if (CLUT == NULL) return FALSE;

    if (!cmsPipelineInsertStage(Dest, cmsAT_END, CLUT)) {
        goto Error;
    }

    if (*dwFlags & cmsFLAGS_CLUT_POST_LINEARIZATION) {

        cmsStage* PostLin = cmsPipelineGetPtrToLastStage(Src);

        if (cmsStageType(PostLin) == cmsSigCurveSetElemType) {

            if (!AllCurvesAreLinear(PostLin)) {

                NewPostLin = cmsStageDup(PostLin);
                if (!cmsPipelineInsertStage(Dest, cmsAT_END, NewPostLin))
                    goto Error;

                cmsPipelineUnlinkStage(Src, cmsAT_END, &KeepPostLin);
            }
        }
    }

    if (!cmsStageSampleCLut16bit(CLUT, XFormSampler16, (void*) Src, 0)) {
Error:
        if (KeepPreLin != NULL) {
            if (!cmsPipelineInsertStage(Src, cmsAT_BEGIN, KeepPreLin)) {
                _cmsAssert(0); // This never happens
            }
        }
        if (KeepPostLin != NULL) {
            if (!cmsPipelineInsertStage(Src, cmsAT_END,   KeepPostLin)) {
                _cmsAssert(0); // This never happens
            }
        }
        cmsPipelineFree(Dest);
        return FALSE;
    }


    if (KeepPreLin != NULL) cmsStageFree(KeepPreLin);
    if (KeepPostLin != NULL) cmsStageFree(KeepPostLin);
    cmsPipelineFree(Src);

    DataCLUT = (_cmsStageCLutData*) CLUT ->Data;

    if (NewPreLin == NULL) DataSetIn = NULL;
    else DataSetIn = ((_cmsStageToneCurvesData*) NewPreLin ->Data) ->TheCurves;

    if (NewPostLin == NULL) DataSetOut = NULL;
    else  DataSetOut = ((_cmsStageToneCurvesData*) NewPostLin ->Data) ->TheCurves;


    if (DataSetIn == NULL && DataSetOut == NULL) {

        _cmsPipelineSetOptimizationParameters(Dest, (_cmsOPTeval16Fn) DataCLUT->Params->Interpolation.Lerp16, DataCLUT->Params, NULL, NULL);
    }
    else {

        p16 = PrelinOpt16alloc(Dest ->ContextID,
            DataCLUT ->Params,
            Dest ->InputChannels,
            DataSetIn,
            Dest ->OutputChannels,
            DataSetOut);

        _cmsPipelineSetOptimizationParameters(Dest, PrelinEval16, (void*) p16, PrelinOpt16free, Prelin16dup);
    }


    if (Intent == INTENT_ABSOLUTE_COLORIMETRIC)
        *dwFlags |= cmsFLAGS_NOWHITEONWHITEFIXUP;

    if (!(*dwFlags & cmsFLAGS_NOWHITEONWHITEFIXUP)) {

        FixWhiteMisalignment(Dest, ColorSpace, OutputColorSpace);
    }

    *Lut = Dest;
    return TRUE;

    cmsUNUSED_PARAMETER(Intent);
}
