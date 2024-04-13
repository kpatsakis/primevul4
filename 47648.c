find_xml_children(xmlNode ** children, xmlNode * root,
                  const char *tag, const char *field, const char *value, gboolean search_matches)
{
    int match_found = 0;

    CRM_CHECK(root != NULL, return FALSE);
    CRM_CHECK(children != NULL, return FALSE);

    if (tag != NULL && safe_str_neq(tag, crm_element_name(root))) {

    } else if (value != NULL && safe_str_neq(value, crm_element_value(root, field))) {

    } else {
        if (*children == NULL) {
            *children = create_xml_node(NULL, __FUNCTION__);
        }
        add_node_copy(*children, root);
        match_found = 1;
    }

    if (search_matches || match_found == 0) {
        xmlNode *child = NULL;

        for (child = __xml_first_child(root); child != NULL; child = __xml_next(child)) {
            match_found += find_xml_children(children, child, tag, field, value, search_matches);
        }
    }

    return match_found;
}
