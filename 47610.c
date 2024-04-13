can_prune_leaf(xmlNode * xml_node)
{
    xmlNode *cIter = NULL;
    xmlAttrPtr pIter = NULL;
    gboolean can_prune = TRUE;
    const char *name = crm_element_name(xml_node);

    if (safe_str_eq(name, XML_TAG_RESOURCE_REF)
        || safe_str_eq(name, XML_CIB_TAG_OBJ_REF)
        || safe_str_eq(name, XML_ACL_TAG_ROLE_REF)
        || safe_str_eq(name, XML_ACL_TAG_ROLE_REFv1)) {
        return FALSE;
    }

    for (pIter = crm_first_attr(xml_node); pIter != NULL; pIter = pIter->next) {
        const char *p_name = (const char *)pIter->name;

        if (strcmp(p_name, XML_ATTR_ID) == 0) {
            continue;
        }
        can_prune = FALSE;
    }

    cIter = __xml_first_child(xml_node);
    while (cIter) {
        xmlNode *child = cIter;

        cIter = __xml_next(cIter);
        if (can_prune_leaf(child)) {
            free_xml(child);
        } else {
            can_prune = FALSE;
        }
    }
    return can_prune;
}
