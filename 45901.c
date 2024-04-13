void FastIdentity16(register const cmsUInt16Number In[],
                    register cmsUInt16Number Out[],
                    register const void* D)
{
    cmsPipeline* Lut = (cmsPipeline*) D;
    cmsUInt32Number i;

    for (i=0; i < Lut ->InputChannels; i++) {
         Out[i] = In[i];
    }
}
