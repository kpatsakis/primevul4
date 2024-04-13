get_schema_name(int version)
{
    if (version < 0 || version >= xml_schema_max) {
        return "unknown";
    }
    return known_schemas[version].name;
}
