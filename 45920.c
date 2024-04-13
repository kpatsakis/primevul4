Prelin8Data* PrelinOpt8alloc(cmsContext ContextID, const cmsInterpParams* p, cmsToneCurve* G[3])
{
    int i;
    cmsUInt16Number Input[3];
    cmsS15Fixed16Number v1, v2, v3;
    Prelin8Data* p8;

    p8 = _cmsMallocZero(ContextID, sizeof(Prelin8Data));
    if (p8 == NULL) return NULL;


    for (i=0; i < 256; i++) {

        if (G != NULL) {

            Input[0] = cmsEvalToneCurve16(G[0], FROM_8_TO_16(i));
            Input[1] = cmsEvalToneCurve16(G[1], FROM_8_TO_16(i));
            Input[2] = cmsEvalToneCurve16(G[2], FROM_8_TO_16(i));
        }
        else {
            Input[0] = FROM_8_TO_16(i);
            Input[1] = FROM_8_TO_16(i);
            Input[2] = FROM_8_TO_16(i);
        }


        v1 = _cmsToFixedDomain(Input[0] * p -> Domain[0]);
        v2 = _cmsToFixedDomain(Input[1] * p -> Domain[1]);
        v3 = _cmsToFixedDomain(Input[2] * p -> Domain[2]);

        p8 ->X0[i] = (p->opta[2] * FIXED_TO_INT(v1));
        p8 ->Y0[i] = (p->opta[1] * FIXED_TO_INT(v2));
        p8 ->Z0[i] = (p->opta[0] * FIXED_TO_INT(v3));

        p8 ->rx[i] = (cmsUInt16Number) FIXED_REST_TO_INT(v1);
        p8 ->ry[i] = (cmsUInt16Number) FIXED_REST_TO_INT(v2);
        p8 ->rz[i] = (cmsUInt16Number) FIXED_REST_TO_INT(v3);
    }

    p8 ->ContextID = ContextID;
    p8 ->p = p;

    return p8;
}
