diff_filter_context(int context, int upper_bound, int lower_bound,
                    xmlNode * xml_node, xmlNode * parent)
{
    xmlNode *us = NULL;
    xmlNode *child = NULL;
    xmlAttrPtr pIter = NULL;
    xmlNode *new_parent = parent;
    const char *name = crm_element_name(xml_node);

    CRM_CHECK(xml_node != NULL && name != NULL, return);

    us = create_xml_node(parent, name);
    for (pIter = crm_first_attr(xml_node); pIter != NULL; pIter = pIter->next) {
        const char *p_name = (const char *)pIter->name;
        const char *p_value = crm_attr_value(pIter);

        lower_bound = context;
        crm_xml_add(us, p_name, p_value);
    }

    if (lower_bound >= 0 || upper_bound >= 0) {
        crm_xml_add(us, XML_ATTR_ID, ID(xml_node));
        new_parent = us;

    } else {
        upper_bound = in_upper_context(0, context, xml_node);
        if (upper_bound >= 0) {
            crm_xml_add(us, XML_ATTR_ID, ID(xml_node));
            new_parent = us;
        } else {
            free_xml(us);
            us = NULL;
        }
    }

    for (child = __xml_first_child(us); child != NULL; child = __xml_next(child)) {
        diff_filter_context(context, upper_bound - 1, lower_bound - 1, child, new_parent);
    }
}
