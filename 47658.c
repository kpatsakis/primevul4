get_schema_root(void)
{
    static const char *base = NULL;

    if (base == NULL) {
        base = getenv("PCMK_schema_directory");
    }
    if (base == NULL || strlen(base) == 0) {
        base = CRM_DTD_DIRECTORY;
    }
    return base;
}
