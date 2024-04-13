cmsSEQ* CMSEXPORT cmsDupProfileSequenceDescription(const cmsSEQ* pseq)
{
    cmsSEQ *NewSeq;
    cmsUInt32Number i;

    if (pseq == NULL)
        return NULL;

    NewSeq = (cmsSEQ*) _cmsMalloc(pseq -> ContextID, sizeof(cmsSEQ));
    if (NewSeq == NULL) return NULL;


    NewSeq -> seq      = (cmsPSEQDESC*) _cmsCalloc(pseq ->ContextID, pseq ->n, sizeof(cmsPSEQDESC));
    if (NewSeq ->seq == NULL) goto Error;

    NewSeq -> ContextID = pseq ->ContextID;
    NewSeq -> n        = pseq ->n;

    for (i=0; i < pseq->n; i++) {

        memmove(&NewSeq ->seq[i].attributes, &pseq ->seq[i].attributes, sizeof(cmsUInt64Number));

        NewSeq ->seq[i].deviceMfg   = pseq ->seq[i].deviceMfg;
        NewSeq ->seq[i].deviceModel = pseq ->seq[i].deviceModel;
        memmove(&NewSeq ->seq[i].ProfileID, &pseq ->seq[i].ProfileID, sizeof(cmsProfileID));
        NewSeq ->seq[i].technology  = pseq ->seq[i].technology;

        NewSeq ->seq[i].Manufacturer = cmsMLUdup(pseq ->seq[i].Manufacturer);
        NewSeq ->seq[i].Model        = cmsMLUdup(pseq ->seq[i].Model);
        NewSeq ->seq[i].Description  = cmsMLUdup(pseq ->seq[i].Description);

    }

    return NewSeq;

Error:

    cmsFreeProfileSequenceDescription(NewSeq);
    return NULL;
}
