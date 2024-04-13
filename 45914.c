void* Prelin8dup(cmsContext ContextID, const void* ptr)
{
    return _cmsDupMem(ContextID, ptr, sizeof(Prelin8Data));
}
