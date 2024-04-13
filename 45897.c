void* DupMatShaper(cmsContext ContextID, const void* Data)
{
    return _cmsDupMem(ContextID, Data, sizeof(MatShaper8Data));
}
