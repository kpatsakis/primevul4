reassemble_pkcs12_name(const char *filename)
{
    char *ret;

    if (asprintf(&ret, "PKCS12:%s", filename) < 0)
        return NULL;
    return ret;
}
