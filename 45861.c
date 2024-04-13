wchar_t* DupWcs(cmsContext ContextID, const wchar_t* ptr)
{
    if (ptr == NULL) return NULL;
    return (wchar_t*) _cmsDupMem(ContextID, ptr, (mywcslen(ptr) + 1) * sizeof(wchar_t));
}
