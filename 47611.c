cli_config_update(xmlNode ** xml, int *best_version, gboolean to_logs)
{
    gboolean rc = TRUE;
    const char *value = crm_element_value(*xml, XML_ATTR_VALIDATION);

    int version = get_schema_version(value);
    int min_version = xml_minimum_schema_index();

    if (version < min_version) {
        xmlNode *converted = NULL;

        converted = copy_xml(*xml);
        update_validation(&converted, &version, 0, TRUE, to_logs);

        value = crm_element_value(converted, XML_ATTR_VALIDATION);
        if (version < min_version) {
            if (to_logs) {
                crm_config_err("Your current configuration could only be upgraded to %s... "
                               "the minimum requirement is %s.\n", crm_str(value),
                               get_schema_name(min_version));
            } else {
                fprintf(stderr, "Your current configuration could only be upgraded to %s... "
                        "the minimum requirement is %s.\n",
                        crm_str(value), get_schema_name(min_version));
            }

            free_xml(converted);
            converted = NULL;
            rc = FALSE;

        } else {
            free_xml(*xml);
            *xml = converted;

            if (version < xml_latest_schema_index()) {
                crm_config_warn("Your configuration was internally updated to %s... "
                                "which is acceptable but not the most recent",
                                get_schema_name(version));

            } else if (to_logs) {
                crm_info("Your configuration was internally updated to the latest version (%s)",
                         get_schema_name(version));
            }
        }

    } else if (version >= get_schema_version("none")) {
        if (to_logs) {
            crm_config_warn("Configuration validation is currently disabled."
                            " It is highly encouraged and prevents many common cluster issues.");

        } else {
            fprintf(stderr, "Configuration validation is currently disabled."
                    " It is highly encouraged and prevents many common cluster issues.\n");
        }
    }

    if (best_version) {
        *best_version = version;
    }

    return rc;
}
