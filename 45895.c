void* CurvesDup(cmsContext ContextID, const void* ptr)
{
    Curves16Data* Data = _cmsDupMem(ContextID, ptr, sizeof(Curves16Data));
    int i;

    if (Data == NULL) return NULL;

    Data ->Curves = _cmsDupMem(ContextID, Data ->Curves, Data ->nCurves * sizeof(cmsUInt16Number*));

    for (i=0; i < Data -> nCurves; i++) {
        Data ->Curves[i] = _cmsDupMem(ContextID, Data ->Curves[i], Data -> nElements * sizeof(cmsUInt16Number));
    }

    return (void*) Data;
}
