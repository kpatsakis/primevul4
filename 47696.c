xml_apply_patchset_v1(xmlNode *xml, xmlNode *patchset, bool check_version) 
{
    int rc = pcmk_ok;
    int root_nodes_seen = 0;
    char *version = crm_element_value_copy(xml, XML_ATTR_CRM_VERSION);

    xmlNode *child_diff = NULL;
    xmlNode *added = find_xml_node(patchset, "diff-added", FALSE);
    xmlNode *removed = find_xml_node(patchset, "diff-removed", FALSE);
    xmlNode *old = copy_xml(xml);

    crm_trace("Substraction Phase");
    for (child_diff = __xml_first_child(removed); child_diff != NULL;
         child_diff = __xml_next(child_diff)) {
        CRM_CHECK(root_nodes_seen == 0, rc = FALSE);
        if (root_nodes_seen == 0) {
            __subtract_xml_object(xml, child_diff);
        }
        root_nodes_seen++;
    }

    if (root_nodes_seen > 1) {
        crm_err("(-) Diffs cannot contain more than one change set... saw %d", root_nodes_seen);
        rc = -ENOTUNIQ;
    }

    root_nodes_seen = 0;
    crm_trace("Addition Phase");
    if (rc == pcmk_ok) {
        xmlNode *child_diff = NULL;

        for (child_diff = __xml_first_child(added); child_diff != NULL;
             child_diff = __xml_next(child_diff)) {
            CRM_CHECK(root_nodes_seen == 0, rc = FALSE);
            if (root_nodes_seen == 0) {
                __add_xml_object(NULL, xml, child_diff);
            }
            root_nodes_seen++;
        }
    }

    if (root_nodes_seen > 1) {
        crm_err("(+) Diffs cannot contain more than one change set... saw %d", root_nodes_seen);
        rc = -ENOTUNIQ;
    }

    purge_diff_markers(xml);       /* Purge prior to checking the digest */

    free_xml(old);
    free(version);
    return rc;
}
