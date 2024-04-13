cmsBool SetMatShaper(cmsPipeline* Dest, cmsToneCurve* Curve1[3], cmsMAT3* Mat, cmsVEC3* Off, cmsToneCurve* Curve2[3], cmsUInt32Number* OutputFormat)
{
    MatShaper8Data* p;
    int i, j;
    cmsBool Is8Bits = _cmsFormatterIs8bit(*OutputFormat);

    p = (MatShaper8Data*) _cmsMalloc(Dest ->ContextID, sizeof(MatShaper8Data));
    if (p == NULL) return FALSE;

    p -> ContextID = Dest -> ContextID;

    FillFirstShaper(p ->Shaper1R, Curve1[0]);
    FillFirstShaper(p ->Shaper1G, Curve1[1]);
    FillFirstShaper(p ->Shaper1B, Curve1[2]);

    FillSecondShaper(p ->Shaper2R, Curve2[0], Is8Bits);
    FillSecondShaper(p ->Shaper2G, Curve2[1], Is8Bits);
    FillSecondShaper(p ->Shaper2B, Curve2[2], Is8Bits);

    for (i=0; i < 3; i++) {
        for (j=0; j < 3; j++) {
            p ->Mat[i][j] = DOUBLE_TO_1FIXED14(Mat->v[i].n[j]);
        }
    }

    for (i=0; i < 3; i++) {

        if (Off == NULL) {
            p ->Off[i] = 0;
        }
        else {
            p ->Off[i] = DOUBLE_TO_1FIXED14(Off->n[i]);
        }
    }

    if (Is8Bits)
        *OutputFormat |= OPTIMIZED_SH(1);

    _cmsPipelineSetOptimizationParameters(Dest, MatShaperEval16, (void*) p, FreeMatShaper, DupMatShaper);
    return TRUE;
}
