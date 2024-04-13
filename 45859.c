cmsBool AddMLUBlock(cmsMLU* mlu, cmsUInt32Number size, const wchar_t *Block,
                     cmsUInt16Number LanguageCode, cmsUInt16Number CountryCode)
{
    cmsUInt32Number Offset;
    cmsUInt8Number* Ptr;

    if (mlu == NULL) return FALSE;

    if (mlu ->UsedEntries >= mlu ->AllocatedEntries) {
        if (!GrowMLUtable(mlu)) return FALSE;
    }

    if (SearchMLUEntry(mlu, LanguageCode, CountryCode) >= 0) return FALSE;  // Only one  is allowed!

    while ((mlu ->PoolSize - mlu ->PoolUsed) < size) {

            if (!GrowMLUpool(mlu)) return FALSE;
    }

    Offset = mlu ->PoolUsed;

    Ptr = (cmsUInt8Number*) mlu ->MemPool;
    if (Ptr == NULL) return FALSE;

    memmove(Ptr + Offset, Block, size);
    mlu ->PoolUsed += size;

    mlu ->Entries[mlu ->UsedEntries].StrW     = Offset;
    mlu ->Entries[mlu ->UsedEntries].Len      = size;
    mlu ->Entries[mlu ->UsedEntries].Country  = CountryCode;
    mlu ->Entries[mlu ->UsedEntries].Language = LanguageCode;
    mlu ->UsedEntries++;

    return TRUE;
}
