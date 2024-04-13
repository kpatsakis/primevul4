update_xml_child(xmlNode * child, xmlNode * to_update)
{
    gboolean can_update = TRUE;
    xmlNode *child_of_child = NULL;

    CRM_CHECK(child != NULL, return FALSE);
    CRM_CHECK(to_update != NULL, return FALSE);

    if (safe_str_neq(crm_element_name(to_update), crm_element_name(child))) {
        can_update = FALSE;

    } else if (safe_str_neq(ID(to_update), ID(child))) {
        can_update = FALSE;

    } else if (can_update) {
#if XML_PARSER_DEBUG
        crm_log_xml_trace(child, "Update match found...");
#endif
        add_xml_object(NULL, child, to_update, FALSE);
    }

    for (child_of_child = __xml_first_child(child); child_of_child != NULL;
         child_of_child = __xml_next(child_of_child)) {
        /* only update the first one */
        if (can_update) {
            break;
        }
        can_update = update_xml_child(child_of_child, to_update);
    }

    return can_update;
}
