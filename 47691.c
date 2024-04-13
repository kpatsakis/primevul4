xml2list(xmlNode * parent)
{
    xmlNode *child = NULL;
    xmlAttrPtr pIter = NULL;
    xmlNode *nvpair_list = NULL;
    GHashTable *nvpair_hash = g_hash_table_new_full(crm_str_hash, g_str_equal,
                                                    g_hash_destroy_str, g_hash_destroy_str);

    CRM_CHECK(parent != NULL, return nvpair_hash);

    nvpair_list = find_xml_node(parent, XML_TAG_ATTRS, FALSE);
    if (nvpair_list == NULL) {
        crm_trace("No attributes in %s", crm_element_name(parent));
        crm_log_xml_trace(parent, "No attributes for resource op");
    }

    crm_log_xml_trace(nvpair_list, "Unpacking");

    for (pIter = crm_first_attr(nvpair_list); pIter != NULL; pIter = pIter->next) {
        const char *p_name = (const char *)pIter->name;
        const char *p_value = crm_attr_value(pIter);

        crm_trace("Added %s=%s", p_name, p_value);

        g_hash_table_insert(nvpair_hash, strdup(p_name), strdup(p_value));
    }

    for (child = __xml_first_child(nvpair_list); child != NULL; child = __xml_next(child)) {
        if (strcmp((const char *)child->name, XML_TAG_PARAM) == 0) {
            const char *key = crm_element_value(child, XML_NVPAIR_ATTR_NAME);
            const char *value = crm_element_value(child, XML_NVPAIR_ATTR_VALUE);

            crm_trace("Added %s=%s", key, value);
            if (key != NULL && value != NULL) {
                g_hash_table_insert(nvpair_hash, strdup(key), strdup(value));
            }
        }
    }

    return nvpair_hash;
}
