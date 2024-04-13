__xml_purge_attributes(xmlNode *xml)
{
    xmlNode *child = NULL;
    xmlAttr *xIter = NULL;
    bool readable_children = FALSE;
    xml_private_t *p = xml->_private;

    if(__xml_acl_mode_test(p->flags, xpf_acl_read)) {
        crm_trace("%s is readable", crm_element_name(xml), ID(xml));
        return TRUE;
    }

    xIter = crm_first_attr(xml);
    while(xIter != NULL) {
        xmlAttr *tmp = xIter;
        const char *prop_name = (const char *)xIter->name;

        xIter = xIter->next;
        if (strcmp(prop_name, XML_ATTR_ID) == 0) {
            continue;
        }

        xmlUnsetProp(xml, tmp->name);
    }

    child = __xml_first_child(xml);
    while ( child != NULL ) {
        xmlNode *tmp = child;

        child = __xml_next(child);
        readable_children |= __xml_purge_attributes(tmp);
    }

    if(readable_children == FALSE) {
        free_xml(xml); /* Nothing readable under here, purge completely */
    }
    return readable_children;
}
