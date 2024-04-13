purge_diff_markers(xmlNode * a_node)
{
    xmlNode *child = NULL;

    CRM_CHECK(a_node != NULL, return);

    xml_remove_prop(a_node, XML_DIFF_MARKER);
    for (child = __xml_first_child(a_node); child != NULL; child = __xml_next(child)) {
        purge_diff_markers(child);
    }
}
