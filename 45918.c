Prelin16Data* PrelinOpt16alloc(cmsContext ContextID,
                               const cmsInterpParams* ColorMap,
                               int nInputs, cmsToneCurve** In,
                               int nOutputs, cmsToneCurve** Out )
{
    int i;
    Prelin16Data* p16 = _cmsMallocZero(ContextID, sizeof(Prelin16Data));
    if (p16 == NULL) return NULL;

    p16 ->nInputs = nInputs;
    p16 -> nOutputs = nOutputs;


    for (i=0; i < nInputs; i++) {

        if (In == NULL) {
            p16 -> ParamsCurveIn16[i] = NULL;
            p16 -> EvalCurveIn16[i] = Eval16nop1D;

        }
        else {
            p16 -> ParamsCurveIn16[i] = In[i] ->InterpParams;
            p16 -> EvalCurveIn16[i] = p16 ->ParamsCurveIn16[i]->Interpolation.Lerp16;
        }
    }

    p16 ->CLUTparams = ColorMap;
    p16 ->EvalCLUT   = ColorMap ->Interpolation.Lerp16;


    p16 -> EvalCurveOut16 = (_cmsInterpFn16*) _cmsCalloc(ContextID, nOutputs, sizeof(_cmsInterpFn16));
    p16 -> ParamsCurveOut16 = (cmsInterpParams**) _cmsCalloc(ContextID, nOutputs, sizeof(cmsInterpParams* ));

    for (i=0; i < nOutputs; i++) {

        if (Out == NULL) {
            p16 ->ParamsCurveOut16[i] = NULL;
            p16 -> EvalCurveOut16[i] = Eval16nop1D;
        }
        else {

            p16 ->ParamsCurveOut16[i] = Out[i] ->InterpParams;
            p16 -> EvalCurveOut16[i] = p16 ->ParamsCurveOut16[i]->Interpolation.Lerp16;
        }
    }

    return p16;
}
