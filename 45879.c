void CMSEXPORT cmsFreeProfileSequenceDescription(cmsSEQ* pseq)
{
    cmsUInt32Number i;

    for (i=0; i < pseq ->n; i++) {
        if (pseq ->seq[i].Manufacturer != NULL) cmsMLUfree(pseq ->seq[i].Manufacturer);
        if (pseq ->seq[i].Model != NULL) cmsMLUfree(pseq ->seq[i].Model);
        if (pseq ->seq[i].Description != NULL) cmsMLUfree(pseq ->seq[i].Description);
    }

    if (pseq ->seq != NULL) _cmsFree(pseq ->ContextID, pseq ->seq);
    _cmsFree(pseq -> ContextID, pseq);
}
