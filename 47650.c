find_xml_node(xmlNode * root, const char *search_path, gboolean must_find)
{
    xmlNode *a_child = NULL;
    const char *name = "NULL";

    if (root != NULL) {
        name = crm_element_name(root);
    }

    if (search_path == NULL) {
        crm_warn("Will never find <NULL>");
        return NULL;
    }

    for (a_child = __xml_first_child(root); a_child != NULL; a_child = __xml_next(a_child)) {
        if (strcmp((const char *)a_child->name, search_path) == 0) {
/* 		crm_trace("returning node (%s).", crm_element_name(a_child)); */
            return a_child;
        }
    }

    if (must_find) {
        crm_warn("Could not find %s in %s.", search_path, name);
    } else if (root != NULL) {
        crm_trace("Could not find %s in %s.", search_path, name);
    } else {
        crm_trace("Could not find %s in <NULL>.", search_path);
    }

    return NULL;
}
