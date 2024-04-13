pkinit_C_LoadModule(const char *modname, CK_FUNCTION_LIST_PTR_PTR p11p)
{
    void *handle;
    CK_RV (*getflist)(CK_FUNCTION_LIST_PTR_PTR);

    pkiDebug("loading module \"%s\"... ", modname);
    handle = dlopen(modname, RTLD_NOW);
    if (handle == NULL) {
        pkiDebug("not found\n");
        return NULL;
    }
    getflist = (CK_RV (*)(CK_FUNCTION_LIST_PTR_PTR)) dlsym(handle, "C_GetFunctionList");
    if (getflist == NULL || (*getflist)(p11p) != CKR_OK) {
        dlclose(handle);
        pkiDebug("failed\n");
        return NULL;
    }
    pkiDebug("ok\n");
    return handle;
}
