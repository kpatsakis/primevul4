cmsBool _cmsOptimizePipeline(cmsPipeline**    PtrLut,
                             int              Intent,
                             cmsUInt32Number* InputFormat,
                             cmsUInt32Number* OutputFormat,
                             cmsUInt32Number* dwFlags)
{
    _cmsOptimizationCollection* Opts;
    cmsBool AnySuccess = FALSE;

    if (*dwFlags & cmsFLAGS_FORCE_CLUT) {

        PreOptimize(*PtrLut);
        return OptimizeByResampling(PtrLut, Intent, InputFormat, OutputFormat, dwFlags);
    }

    if ((*PtrLut) ->Elements == NULL) {
        _cmsPipelineSetOptimizationParameters(*PtrLut, FastIdentity16, (void*) *PtrLut, NULL, NULL);
        return TRUE;
    }

    AnySuccess = PreOptimize(*PtrLut);

    if ((*PtrLut) ->Elements == NULL) {
        _cmsPipelineSetOptimizationParameters(*PtrLut, FastIdentity16, (void*) *PtrLut, NULL, NULL);
        return TRUE;
    }

    if (*dwFlags & cmsFLAGS_NOOPTIMIZE)
        return FALSE;

    for (Opts = OptimizationCollection;
         Opts != NULL;
         Opts = Opts ->Next) {

            if (Opts ->OptimizePtr(PtrLut, Intent, InputFormat, OutputFormat, dwFlags)) {

                return TRUE;    // Optimized!
            }
    }

    return AnySuccess;
}
