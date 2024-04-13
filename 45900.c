void FastEvaluateCurves8(register const cmsUInt16Number In[],
                          register cmsUInt16Number Out[],
                          register const void* D)
{
    Curves16Data* Data = (Curves16Data*) D;
    cmsUInt8Number x;
    int i;

    for (i=0; i < Data ->nCurves; i++) {

         x = (In[i] >> 8);
         Out[i] = Data -> Curves[i][x];
    }
}
