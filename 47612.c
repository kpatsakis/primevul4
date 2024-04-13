copy_in_properties(xmlNode * target, xmlNode * src)
{
    if (src == NULL) {
        crm_warn("No node to copy properties from");

    } else if (target == NULL) {
        crm_err("No node to copy properties into");

    } else {
        xmlAttrPtr pIter = NULL;

        for (pIter = crm_first_attr(src); pIter != NULL; pIter = pIter->next) {
            const char *p_name = (const char *)pIter->name;
            const char *p_value = crm_attr_value(pIter);

            expand_plus_plus(target, p_name, p_value);
        }
    }

    return;
}
