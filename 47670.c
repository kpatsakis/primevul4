replace_xml_child(xmlNode * parent, xmlNode * child, xmlNode * update, gboolean delete_only)
{
    gboolean can_delete = FALSE;
    xmlNode *child_of_child = NULL;

    const char *up_id = NULL;
    const char *child_id = NULL;
    const char *right_val = NULL;

    CRM_CHECK(child != NULL, return FALSE);
    CRM_CHECK(update != NULL, return FALSE);

    up_id = ID(update);
    child_id = ID(child);

    if (up_id == NULL || (child_id && strcmp(child_id, up_id) == 0)) {
        can_delete = TRUE;
    }
    if (safe_str_neq(crm_element_name(update), crm_element_name(child))) {
        can_delete = FALSE;
    }
    if (can_delete && delete_only) {
        xmlAttrPtr pIter = NULL;

        for (pIter = crm_first_attr(update); pIter != NULL; pIter = pIter->next) {
            const char *p_name = (const char *)pIter->name;
            const char *p_value = crm_attr_value(pIter);

            right_val = crm_element_value(child, p_name);
            if (safe_str_neq(p_value, right_val)) {
                can_delete = FALSE;
            }
        }
    }

    if (can_delete && parent != NULL) {
        crm_log_xml_trace(child, "Delete match found...");
        if (delete_only || update == NULL) {
            free_xml(child);

        } else {
            xmlNode *tmp = copy_xml(update);
            xmlDoc *doc = tmp->doc;
            xmlNode *old = NULL;

            xml_accept_changes(tmp);
            old = xmlReplaceNode(child, tmp);

            if(xml_tracking_changes(tmp)) {
                /* Replaced sections may have included relevant ACLs */
                __xml_acl_apply(tmp);
            }

            xml_calculate_changes(old, tmp);
            xmlDocSetRootElement(doc, old);
            free_xml(old);
        }
        child = NULL;
        return TRUE;

    } else if (can_delete) {
        crm_log_xml_debug(child, "Cannot delete the search root");
        can_delete = FALSE;
    }

    child_of_child = __xml_first_child(child);
    while (child_of_child) {
        xmlNode *next = __xml_next(child_of_child);

        can_delete = replace_xml_child(child, child_of_child, update, delete_only);

        /* only delete the first one */
        if (can_delete) {
            child_of_child = NULL;
        } else {
            child_of_child = next;
        }
    }

    return can_delete;
}
