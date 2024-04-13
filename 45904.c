cmsBool FixWhiteMisalignment(cmsPipeline* Lut, cmsColorSpaceSignature EntryColorSpace, cmsColorSpaceSignature ExitColorSpace)
{
    cmsUInt16Number *WhitePointIn, *WhitePointOut;
    cmsUInt16Number  WhiteIn[cmsMAXCHANNELS], WhiteOut[cmsMAXCHANNELS], ObtainedOut[cmsMAXCHANNELS];
    cmsUInt32Number i, nOuts, nIns;
    cmsStage *PreLin = NULL, *CLUT = NULL, *PostLin = NULL;

    if (!_cmsEndPointsBySpace(EntryColorSpace,
        &WhitePointIn, NULL, &nIns)) return FALSE;

    if (!_cmsEndPointsBySpace(ExitColorSpace,
        &WhitePointOut, NULL, &nOuts)) return FALSE;

    if (Lut ->InputChannels != nIns) return FALSE;
    if (Lut ->OutputChannels != nOuts) return FALSE;

    cmsPipelineEval16(WhitePointIn, ObtainedOut, Lut);

    if (WhitesAreEqual(nOuts, WhitePointOut, ObtainedOut)) return TRUE; // whites already match

    if (!cmsPipelineCheckAndRetreiveStages(Lut, 3, cmsSigCurveSetElemType, cmsSigCLutElemType, cmsSigCurveSetElemType, &PreLin, &CLUT, &PostLin))
        if (!cmsPipelineCheckAndRetreiveStages(Lut, 2, cmsSigCurveSetElemType, cmsSigCLutElemType, &PreLin, &CLUT))
            if (!cmsPipelineCheckAndRetreiveStages(Lut, 2, cmsSigCLutElemType, cmsSigCurveSetElemType, &CLUT, &PostLin))
                if (!cmsPipelineCheckAndRetreiveStages(Lut, 1, cmsSigCLutElemType, &CLUT))
                    return FALSE;

    if (PreLin) {

        cmsToneCurve** Curves = _cmsStageGetPtrToCurveSet(PreLin);

        for (i=0; i < nIns; i++) {
            WhiteIn[i] = cmsEvalToneCurve16(Curves[i], WhitePointIn[i]);
        }
    }
    else {
        for (i=0; i < nIns; i++)
            WhiteIn[i] = WhitePointIn[i];
    }

    if (PostLin) {

        cmsToneCurve** Curves = _cmsStageGetPtrToCurveSet(PostLin);

        for (i=0; i < nOuts; i++) {

            cmsToneCurve* InversePostLin = cmsReverseToneCurve(Curves[i]);
            WhiteOut[i] = cmsEvalToneCurve16(InversePostLin, WhitePointOut[i]);
            cmsFreeToneCurve(InversePostLin);
        }
    }
    else {
        for (i=0; i < nOuts; i++)
            WhiteOut[i] = WhitePointOut[i];
    }

    PatchLUT(CLUT, WhiteIn, WhiteOut, nOuts, nIns);

    return TRUE;
}
