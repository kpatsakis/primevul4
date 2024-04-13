cmsMLU* CMSEXPORT cmsMLUdup(const cmsMLU* mlu)
{
    cmsMLU* NewMlu = NULL;

    if (mlu == NULL) return NULL;

    NewMlu = cmsMLUalloc(mlu ->ContextID, mlu ->UsedEntries);
    if (NewMlu == NULL) return NULL;

    if (NewMlu ->AllocatedEntries < mlu ->UsedEntries)
        goto Error;

    if (NewMlu ->Entries == NULL || mlu ->Entries == NULL)  goto Error;

    memmove(NewMlu ->Entries, mlu ->Entries, mlu ->UsedEntries * sizeof(_cmsMLUentry));
    NewMlu ->UsedEntries = mlu ->UsedEntries;

    if (mlu ->PoolUsed == 0) {
        NewMlu ->MemPool = NULL;
    }
    else {
        NewMlu ->MemPool = _cmsMalloc(mlu ->ContextID, mlu ->PoolUsed);
        if (NewMlu ->MemPool == NULL) goto Error;
    }

    NewMlu ->PoolSize = mlu ->PoolUsed;

    if (NewMlu ->MemPool == NULL || mlu ->MemPool == NULL) goto Error;

    memmove(NewMlu ->MemPool, mlu->MemPool, mlu ->PoolUsed);
    NewMlu ->PoolUsed = mlu ->PoolUsed;

    return NewMlu;

Error:

    if (NewMlu != NULL) cmsMLUfree(NewMlu);
    return NULL;
}
