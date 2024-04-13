crm_element_value(xmlNode * data, const char *name)
{
    xmlAttr *attr = NULL;

    if (data == NULL) {
        crm_err("Couldn't find %s in NULL", name ? name : "<null>");
        CRM_LOG_ASSERT(data != NULL);
        return NULL;

    } else if (name == NULL) {
        crm_err("Couldn't find NULL in %s", crm_element_name(data));
        return NULL;
    }

    attr = xmlHasProp(data, (const xmlChar *)name);
    if (attr == NULL || attr->children == NULL) {
        return NULL;
    }
    return (const char *)attr->children->content;
}
