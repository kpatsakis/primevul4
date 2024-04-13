static void __xml_schema_add(
    int type, float version, const char *name, const char *location, const char *transform, int after_transform)
{
    int last = xml_schema_max;

    xml_schema_max++;
    known_schemas = realloc_safe(known_schemas, xml_schema_max*sizeof(struct schema_s));
    CRM_ASSERT(known_schemas != NULL);
    memset(known_schemas+last, 0, sizeof(struct schema_s));
    known_schemas[last].type = type;
    known_schemas[last].after_transform = after_transform;

    if(version > 0.0) {
        known_schemas[last].version = version;
        known_schemas[last].name = crm_strdup_printf("pacemaker-%.1f", version);
        known_schemas[last].location = crm_strdup_printf("%s.rng", known_schemas[last].name);

    } else {
        char dummy[1024];
        CRM_ASSERT(name);
        CRM_ASSERT(location);
        sscanf(name, "%[^-]-%f", dummy, &version);
        known_schemas[last].version = version;
        known_schemas[last].name = strdup(name);
        known_schemas[last].location = strdup(location);
    }

    if(transform) {
        known_schemas[last].transform = strdup(transform);
    }
    if(after_transform == 0) {
        after_transform = xml_schema_max;
    }
    known_schemas[last].after_transform = after_transform;

    if(known_schemas[last].after_transform < 0) {
        crm_debug("Added supported schema %d: %s (%s)",
                  last, known_schemas[last].name, known_schemas[last].location);

    } else if(known_schemas[last].transform) {
        crm_debug("Added supported schema %d: %s (%s upgrades to %d with %s)",
                  last, known_schemas[last].name, known_schemas[last].location,
                  known_schemas[last].after_transform,
                  known_schemas[last].transform);

    } else {
        crm_debug("Added supported schema %d: %s (%s upgrades to %d)",
                  last, known_schemas[last].name, known_schemas[last].location,
                  known_schemas[last].after_transform);
    }
}
