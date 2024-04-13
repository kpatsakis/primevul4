update_validation(xmlNode ** xml_blob, int *best, int max, gboolean transform, gboolean to_logs)
{
    xmlNode *xml = NULL;
    char *value = NULL;
    int max_stable_schemas = xml_latest_schema_index();
    int lpc = 0, match = -1, rc = pcmk_ok;

    CRM_CHECK(best != NULL, return -EINVAL);
    CRM_CHECK(xml_blob != NULL, return -EINVAL);
    CRM_CHECK(*xml_blob != NULL, return -EINVAL);

    *best = 0;
    xml = *xml_blob;
    value = crm_element_value_copy(xml, XML_ATTR_VALIDATION);

    if (value != NULL) {
        match = get_schema_version(value);

        lpc = match;
        if (lpc >= 0 && transform == FALSE) {
            lpc++;

        } else if (lpc < 0) {
            crm_debug("Unknown validation type");
            lpc = 0;
        }
    }

    if (match >= max_stable_schemas) {
        /* nothing to do */
        free(value);
        *best = match;
        return pcmk_ok;
    }

    while(lpc <= max_stable_schemas) {
        gboolean valid = TRUE;

        crm_debug("Testing '%s' validation (%d of %d)",
                  known_schemas[lpc].name ? known_schemas[lpc].name : "<unset>",
                  lpc, max_stable_schemas);
        valid = validate_with(xml, lpc, to_logs);

        if (valid) {
            *best = lpc;
        } else {
            crm_trace("%s validation failed", known_schemas[lpc].name ? known_schemas[lpc].name : "<unset>");
        }

        if (valid && transform) {
            xmlNode *upgrade = NULL;
            int next = known_schemas[lpc].after_transform;

            if (next < 0) {
                crm_trace("Stopping at %s", known_schemas[lpc].name);
                break;

            } else if (max > 0 && lpc == max) {
                crm_trace("Upgrade limit reached at %s (lpc=%d, next=%d, max=%d)",
                          known_schemas[lpc].name, lpc, next, max);
                break;

            } else if (max > 0 && next > max) {
                crm_debug("Upgrade limit reached at %s (lpc=%d, next=%d, max=%d)",
                          known_schemas[lpc].name, lpc, next, max);
                break;

            } else if (known_schemas[lpc].transform == NULL) {
                crm_notice("%s-style configuration is also valid for %s",
                           known_schemas[lpc].name, known_schemas[next].name);

                if (validate_with(xml, next, to_logs)) {
                    crm_debug("Configuration valid for schema: %s", known_schemas[next].name);
                    lpc = next;
                    *best = next;
                    rc = pcmk_ok;

                } else {
                    crm_info("Configuration not valid for schema: %s", known_schemas[next].name);
                }

            } else {
                crm_debug("Upgrading %s-style configuration to %s with %s",
                           known_schemas[lpc].name, known_schemas[next].name,
                           known_schemas[lpc].transform ? known_schemas[lpc].transform : "no-op");

#if HAVE_LIBXSLT
                upgrade = apply_transformation(xml, known_schemas[lpc].transform);
#endif
                if (upgrade == NULL) {
                    crm_err("Transformation %s failed", known_schemas[lpc].transform);
                    rc = -pcmk_err_transform_failed;

                } else if (validate_with(upgrade, next, to_logs)) {
                    crm_info("Transformation %s successful", known_schemas[lpc].transform);
                    lpc = next;
                    *best = next;
                    free_xml(xml);
                    xml = upgrade;
                    rc = pcmk_ok;

                } else {
                    crm_err("Transformation %s did not produce a valid configuration",
                            known_schemas[lpc].transform);
                    crm_log_xml_info(upgrade, "transform:bad");
                    free_xml(upgrade);
                    rc = -pcmk_err_schema_validation;
                }
            }
        }
    }

    if (*best > match) {
        crm_info("%s the configuration from %s to %s",
                   transform?"Transformed":"Upgraded",
                   value ? value : "<none>", known_schemas[*best].name);
        crm_xml_add(xml, XML_ATTR_VALIDATION, known_schemas[*best].name);
    }

    *xml_blob = xml;
    free(value);
    return rc;
}
