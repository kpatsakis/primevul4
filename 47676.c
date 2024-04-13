sorted_xml(xmlNode * input, xmlNode * parent, gboolean recursive)
{
    xmlNode *child = NULL;
    GListPtr sorted = NULL;
    GListPtr unsorted = NULL;
    name_value_t *pair = NULL;
    xmlNode *result = NULL;
    const char *name = NULL;
    xmlAttrPtr pIter = NULL;

    CRM_CHECK(input != NULL, return NULL);

    name = crm_element_name(input);
    CRM_CHECK(name != NULL, return NULL);

    result = create_xml_node(parent, name);

    for (pIter = crm_first_attr(input); pIter != NULL; pIter = pIter->next) {
        const char *p_name = (const char *)pIter->name;
        const char *p_value = crm_attr_value(pIter);

        pair = calloc(1, sizeof(name_value_t));
        pair->name = p_name;
        pair->value = p_value;
        unsorted = g_list_prepend(unsorted, pair);
        pair = NULL;
    }

    sorted = g_list_sort(unsorted, sort_pairs);
    g_list_foreach(sorted, dump_pair, result);
    g_list_free_full(sorted, free);

    for (child = __xml_first_child(input); child != NULL; child = __xml_next(child)) {
        if (recursive) {
            sorted_xml(child, result, recursive);
        } else {
            add_node_copy(result, child);
        }
    }

    return result;
}
