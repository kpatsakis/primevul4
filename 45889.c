cmsUInt32Number CMSEXPORT cmsMLUtranslationsCount(const cmsMLU* mlu)
{
    if (mlu == NULL) return 0;
    return mlu->UsedEntries;
}
