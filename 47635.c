diff_xml_object(xmlNode * old, xmlNode * new, gboolean suppress)
{
    xmlNode *tmp1 = NULL;
    xmlNode *diff = create_xml_node(NULL, "diff");
    xmlNode *removed = create_xml_node(diff, "diff-removed");
    xmlNode *added = create_xml_node(diff, "diff-added");

    crm_xml_add(diff, XML_ATTR_CRM_VERSION, CRM_FEATURE_SET);

    tmp1 = subtract_xml_object(removed, old, new, FALSE, NULL, "removed:top");
    if (suppress && tmp1 != NULL && can_prune_leaf(tmp1)) {
        free_xml(tmp1);
    }

    tmp1 = subtract_xml_object(added, new, old, TRUE, NULL, "added:top");
    if (suppress && tmp1 != NULL && can_prune_leaf(tmp1)) {
        free_xml(tmp1);
    }

    if (added->children == NULL && removed->children == NULL) {
        free_xml(diff);
        diff = NULL;
    }

    return diff;
}
