void PrelinEval16(register const cmsUInt16Number Input[],
                  register cmsUInt16Number Output[],
                  register const void* D)
{
    Prelin16Data* p16 = (Prelin16Data*) D;
    cmsUInt16Number  StageABC[MAX_INPUT_DIMENSIONS];
    cmsUInt16Number  StageDEF[cmsMAXCHANNELS];
    int i;

    for (i=0; i < p16 ->nInputs; i++) {

        p16 ->EvalCurveIn16[i](&Input[i], &StageABC[i], p16 ->ParamsCurveIn16[i]);
    }

    p16 ->EvalCLUT(StageABC, StageDEF, p16 ->CLUTparams);

    for (i=0; i < p16 ->nOutputs; i++) {

        p16 ->EvalCurveOut16[i](&StageDEF[i], &Output[i], p16 ->ParamsCurveOut16[i]);
    }
}
