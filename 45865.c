cmsBool GrowMLUpool(cmsMLU* mlu)
{
    cmsUInt32Number size;
    void *NewPtr;

    if (mlu == NULL) return FALSE;

    if (mlu ->PoolSize == 0)
        size = 256;
    else
        size = mlu ->PoolSize * 2;

    if (size < mlu ->PoolSize) return FALSE;

    NewPtr = _cmsRealloc(mlu ->ContextID, mlu ->MemPool, size);
    if (NewPtr == NULL) return FALSE;


    mlu ->MemPool  = NewPtr;
    mlu ->PoolSize = size;

    return TRUE;
}
