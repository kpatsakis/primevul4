expand_idref(xmlNode * input, xmlNode * top)
{
    const char *tag = NULL;
    const char *ref = NULL;
    xmlNode *result = input;
    char *xpath_string = NULL;

    if (result == NULL) {
        return NULL;

    } else if (top == NULL) {
        top = input;
    }

    tag = crm_element_name(result);
    ref = crm_element_value(result, XML_ATTR_IDREF);

    if (ref != NULL) {
        int xpath_max = 512, offset = 0;

        xpath_string = calloc(1, xpath_max);

        offset += snprintf(xpath_string + offset, xpath_max - offset, "//%s[@id='%s']", tag, ref);
        CRM_LOG_ASSERT(offset > 0);

        result = get_xpath_object(xpath_string, top, LOG_ERR);
        if (result == NULL) {
            char *nodePath = (char *)xmlGetNodePath(top);

            crm_err("No match for %s found in %s: Invalid configuration", xpath_string,
                    crm_str(nodePath));
            free(nodePath);
        }
    }

    free(xpath_string);
    return result;
}
