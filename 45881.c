cmsMLU* CMSEXPORT cmsMLUalloc(cmsContext ContextID, cmsUInt32Number nItems)
{
    cmsMLU* mlu;

    if (nItems <= 0) nItems = 2;

    mlu = (cmsMLU*) _cmsMallocZero(ContextID, sizeof(cmsMLU));
    if (mlu == NULL) return NULL;

    mlu ->ContextID = ContextID;

    mlu ->Entries = (_cmsMLUentry*) _cmsCalloc(ContextID, nItems, sizeof(_cmsMLUentry));
    if (mlu ->Entries == NULL) {
        _cmsFree(ContextID, mlu);
        return NULL;
    }

    mlu ->AllocatedEntries    = nItems;
    mlu ->UsedEntries         = 0;

    return mlu;
}
