void* Prelin16dup(cmsContext ContextID, const void* ptr)
{
    Prelin16Data* p16 = (Prelin16Data*) ptr;
    Prelin16Data* Duped = _cmsDupMem(ContextID, p16, sizeof(Prelin16Data));

    if (Duped == NULL) return NULL;

    Duped ->EvalCurveOut16   = _cmsDupMem(ContextID, p16 ->EvalCurveOut16, p16 ->nOutputs * sizeof(_cmsInterpFn16));
    Duped ->ParamsCurveOut16 = _cmsDupMem(ContextID, p16 ->ParamsCurveOut16, p16 ->nOutputs * sizeof(cmsInterpParams* ));

    return Duped;
}
