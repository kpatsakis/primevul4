pkinit_C_UnloadModule(void *handle)
{
    dlclose(handle);
    return CKR_OK;
}
