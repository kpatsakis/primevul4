get_schema_version(const char *name)
{
    int lpc = 0;

    if(name == NULL) {
        name = "none";
    }
    for (; lpc < xml_schema_max; lpc++) {
        if (safe_str_eq(name, known_schemas[lpc].name)) {
            return lpc;
        }
    }
    return -1;
}
