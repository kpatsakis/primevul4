void MatShaperEval16(register const cmsUInt16Number In[],
                     register cmsUInt16Number Out[],
                     register const void* D)
{
    MatShaper8Data* p = (MatShaper8Data*) D;
    cmsS1Fixed14Number l1, l2, l3, r, g, b;
    cmsUInt32Number ri, gi, bi;

    ri = In[0] & 0xFF;
    gi = In[1] & 0xFF;
    bi = In[2] & 0xFF;

    r = p->Shaper1R[ri];
    g = p->Shaper1G[gi];
    b = p->Shaper1B[bi];

    l1 =  (p->Mat[0][0] * r + p->Mat[0][1] * g + p->Mat[0][2] * b + p->Off[0] + 0x2000) >> 14;
    l2 =  (p->Mat[1][0] * r + p->Mat[1][1] * g + p->Mat[1][2] * b + p->Off[1] + 0x2000) >> 14;
    l3 =  (p->Mat[2][0] * r + p->Mat[2][1] * g + p->Mat[2][2] * b + p->Off[2] + 0x2000) >> 14;

    ri = (l1 < 0) ? 0 : ((l1 > 16384) ? 16384 : l1);
    gi = (l2 < 0) ? 0 : ((l2 > 16384) ? 16384 : l2);
    bi = (l3 < 0) ? 0 : ((l3 > 16384) ? 16384 : l3);

    Out[0] = p->Shaper2R[ri];
    Out[1] = p->Shaper2G[gi];
    Out[2] = p->Shaper2B[bi];

}
