apply_xml_diff(xmlNode * old, xmlNode * diff, xmlNode ** new)
{
    gboolean result = TRUE;
    int root_nodes_seen = 0;
    static struct qb_log_callsite *digest_cs = NULL;
    const char *digest = crm_element_value(diff, XML_ATTR_DIGEST);
    const char *version = crm_element_value(diff, XML_ATTR_CRM_VERSION);

    xmlNode *child_diff = NULL;
    xmlNode *added = find_xml_node(diff, "diff-added", FALSE);
    xmlNode *removed = find_xml_node(diff, "diff-removed", FALSE);

    CRM_CHECK(new != NULL, return FALSE);
    if (digest_cs == NULL) {
        digest_cs =
            qb_log_callsite_get(__func__, __FILE__, "diff-digest", LOG_TRACE, __LINE__,
                                crm_trace_nonlog);
    }

    crm_trace("Substraction Phase");
    for (child_diff = __xml_first_child(removed); child_diff != NULL;
         child_diff = __xml_next(child_diff)) {
        CRM_CHECK(root_nodes_seen == 0, result = FALSE);
        if (root_nodes_seen == 0) {
            *new = subtract_xml_object(NULL, old, child_diff, FALSE, NULL, NULL);
        }
        root_nodes_seen++;
    }

    if (root_nodes_seen == 0) {
        *new = copy_xml(old);

    } else if (root_nodes_seen > 1) {
        crm_err("(-) Diffs cannot contain more than one change set..." " saw %d", root_nodes_seen);
        result = FALSE;
    }

    root_nodes_seen = 0;
    crm_trace("Addition Phase");
    if (result) {
        xmlNode *child_diff = NULL;

        for (child_diff = __xml_first_child(added); child_diff != NULL;
             child_diff = __xml_next(child_diff)) {
            CRM_CHECK(root_nodes_seen == 0, result = FALSE);
            if (root_nodes_seen == 0) {
                add_xml_object(NULL, *new, child_diff, TRUE);
            }
            root_nodes_seen++;
        }
    }

    if (root_nodes_seen > 1) {
        crm_err("(+) Diffs cannot contain more than one change set..." " saw %d", root_nodes_seen);
        result = FALSE;

    } else if (result && digest) {
        char *new_digest = NULL;

        purge_diff_markers(*new);       /* Purge now so the diff is ok */
        new_digest = calculate_xml_versioned_digest(*new, FALSE, TRUE, version);
        if (safe_str_neq(new_digest, digest)) {
            crm_info("Digest mis-match: expected %s, calculated %s", digest, new_digest);
            result = FALSE;

            crm_trace("%p %0.6x", digest_cs, digest_cs ? digest_cs->targets : 0);
            if (digest_cs && digest_cs->targets) {
                save_xml_to_file(old, "diff:original", NULL);
                save_xml_to_file(diff, "diff:input", NULL);
                save_xml_to_file(*new, "diff:new", NULL);
            }

        } else {
            crm_trace("Digest matched: expected %s, calculated %s", digest, new_digest);
        }
        free(new_digest);

    } else if (result) {
        purge_diff_markers(*new);       /* Purge now so the diff is ok */
    }

    return result;
}
