void FastEvaluateCurves16(register const cmsUInt16Number In[],
                          register cmsUInt16Number Out[],
                          register const void* D)
{
    Curves16Data* Data = (Curves16Data*) D;
    int i;

    for (i=0; i < Data ->nCurves; i++) {
         Out[i] = Data -> Curves[i][In[i]];
    }
}
