crm_xml_add(xmlNode * node, const char *name, const char *value)
{
    bool dirty = FALSE;
    xmlAttr *attr = NULL;

    CRM_CHECK(node != NULL, return NULL);
    CRM_CHECK(name != NULL, return NULL);

    if (value == NULL) {
        return NULL;
    }
#if XML_PARANOIA_CHECKS
    {
        const char *old_value = NULL;

        old_value = crm_element_value(node, name);

        /* Could be re-setting the same value */
        CRM_CHECK(old_value != value, crm_err("Cannot reset %s with crm_xml_add(%s)", name, value);
                  return value);
    }
#endif

    if(TRACKING_CHANGES(node)) {
        const char *old = crm_element_value(node, name);

        if(old == NULL || value == NULL || strcmp(old, value) != 0) {
            dirty = TRUE;
        }
    }

    if(dirty && __xml_acl_check(node, name, xpf_acl_create) == FALSE) {
        crm_trace("Cannot add %s=%s to %s", name, value, node->name);
        return NULL;
    }

    attr = xmlSetProp(node, (const xmlChar *)name, (const xmlChar *)value);
    if(dirty) {
        crm_attr_dirty(attr);
    }

    CRM_CHECK(attr && attr->children && attr->children->content, return NULL);
    return (char *)attr->children->content;
}
