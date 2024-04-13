void CMSEXPORT cmsMLUfree(cmsMLU* mlu)
{
    if (mlu) {

        if (mlu -> Entries) _cmsFree(mlu ->ContextID, mlu->Entries);
        if (mlu -> MemPool) _cmsFree(mlu ->ContextID, mlu->MemPool);

        _cmsFree(mlu ->ContextID, mlu);
    }
}
