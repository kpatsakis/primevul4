void PrelinOpt16free(cmsContext ContextID, void* ptr)
{
    Prelin16Data* p16 = (Prelin16Data*) ptr;

    _cmsFree(ContextID, p16 ->EvalCurveOut16);
    _cmsFree(ContextID, p16 ->ParamsCurveOut16);

    _cmsFree(ContextID, p16);
}
