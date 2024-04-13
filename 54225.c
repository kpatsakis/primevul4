cmsPipeline*  BlackPreservingKOnlyIntents(cmsContext     ContextID,
                                          cmsUInt32Number nProfiles,
                                          cmsUInt32Number TheIntents[],
                                          cmsHPROFILE     hProfiles[],
                                          cmsBool         BPC[],
                                          cmsFloat64Number AdaptationStates[],
                                          cmsUInt32Number dwFlags)
{
    GrayOnlyParams  bp;
    cmsPipeline*    Result;
    cmsUInt32Number ICCIntents[256];
    cmsStage*         CLUT;
    cmsUInt32Number i, nGridPoints;


    if (nProfiles < 1 || nProfiles > 255) return NULL;

    for (i=0; i < nProfiles; i++)
        ICCIntents[i] = TranslateNonICCIntents(TheIntents[i]);

    if (cmsGetColorSpace(hProfiles[0]) != cmsSigCmykData ||
        cmsGetColorSpace(hProfiles[nProfiles-1]) != cmsSigCmykData)
           return DefaultICCintents(ContextID, nProfiles, ICCIntents, hProfiles, BPC, AdaptationStates, dwFlags);

    memset(&bp, 0, sizeof(bp));

    Result = cmsPipelineAlloc(ContextID, 4, 4);
    if (Result == NULL) return NULL;

    bp.cmyk2cmyk = DefaultICCintents(ContextID,
        nProfiles,
        ICCIntents,
        hProfiles,
        BPC,
        AdaptationStates,
        dwFlags);

    if (bp.cmyk2cmyk == NULL) goto Error;

    bp.KTone = _cmsBuildKToneCurve(ContextID,
        4096,
        nProfiles,
        ICCIntents,
        hProfiles,
        BPC,
        AdaptationStates,
        dwFlags);

    if (bp.KTone == NULL) goto Error;


    nGridPoints = _cmsReasonableGridpointsByColorspace(cmsSigCmykData, dwFlags);

    CLUT = cmsStageAllocCLut16bit(ContextID, nGridPoints, 4, 4, NULL);
    if (CLUT == NULL) goto Error;

    if (!cmsPipelineInsertStage(Result, cmsAT_BEGIN, CLUT))
        goto Error;

    if (!cmsStageSampleCLut16bit(CLUT, BlackPreservingGrayOnlySampler, (void*) &bp, 0))
        goto Error;

    cmsPipelineFree(bp.cmyk2cmyk);
    cmsFreeToneCurve(bp.KTone);

    return Result;

Error:

    if (bp.cmyk2cmyk != NULL) cmsPipelineFree(bp.cmyk2cmyk);
    if (bp.KTone != NULL)  cmsFreeToneCurve(bp.KTone);
    if (Result != NULL) cmsPipelineFree(Result);
    return NULL;

}
