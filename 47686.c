validate_xml(xmlNode * xml_blob, const char *validation, gboolean to_logs)
{
    int version = 0;

    if (validation == NULL) {
        validation = crm_element_value(xml_blob, XML_ATTR_VALIDATION);
    }

    if (validation == NULL) {
        int lpc = 0;
        bool valid = FALSE;

        validation = crm_element_value(xml_blob, "ignore-dtd");
        if (crm_is_true(validation)) {
            /* Legacy compatibilty */
            crm_xml_add(xml_blob, XML_ATTR_VALIDATION, "none");
            return TRUE;
        }

        /* Work it out */
        for (lpc = 0; lpc < xml_schema_max; lpc++) {
            if(validate_with(xml_blob, lpc, FALSE)) {
                valid = TRUE;
                crm_xml_add(xml_blob, XML_ATTR_VALIDATION, known_schemas[lpc].name);
                crm_info("XML validated against %s", known_schemas[lpc].name);
                if(known_schemas[lpc].after_transform == 0) {
                    break;
                }
            }
        }

        return valid;
    }

    version = get_schema_version(validation);
    if (strcmp(validation, "none") == 0) {
        return TRUE;

    } else if(version < xml_schema_max) {
        return validate_with(xml_blob, version, to_logs);

    }

    crm_err("Unknown validator: %s", validation);
    return FALSE;
}
