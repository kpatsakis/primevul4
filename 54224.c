int BlackPreservingGrayOnlySampler(register const cmsUInt16Number In[], register cmsUInt16Number Out[], register void* Cargo)
{
    GrayOnlyParams* bp = (GrayOnlyParams*) Cargo;

    if (In[0] == 0 && In[1] == 0 && In[2] == 0) {

        Out[0] = Out[1] = Out[2] = 0;
        Out[3] = cmsEvalToneCurve16(bp->KTone, In[3]);
        return TRUE;
    }

    bp ->cmyk2cmyk ->Eval16Fn(In, Out, bp ->cmyk2cmyk->Data);
    return TRUE;
}
