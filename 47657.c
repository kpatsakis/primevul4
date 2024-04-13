get_schema_path(const char *name, const char *file)
{
    const char *base = get_schema_root();

    if(file) {
        return crm_strdup_printf("%s/%s", base, file);
    }
    return crm_strdup_printf("%s/%s.rng", base, name);
}
