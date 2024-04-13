xml_log_patchset(uint8_t log_level, const char *function, xmlNode * patchset)
{
    int format = 1;
    xmlNode *child = NULL;
    xmlNode *added = NULL;
    xmlNode *removed = NULL;
    gboolean is_first = TRUE;

    int add[] = { 0, 0, 0 };
    int del[] = { 0, 0, 0 };

    const char *fmt = NULL;
    const char *digest = NULL;
    int options = xml_log_option_formatted;

    static struct qb_log_callsite *patchset_cs = NULL;

    if (patchset_cs == NULL) {
        patchset_cs = qb_log_callsite_get(function, __FILE__, "xml-patchset", log_level, __LINE__, 0);
    }

    if (patchset == NULL) {
        crm_trace("Empty patch");
        return;

    } else if (log_level == 0) {
        /* Log to stdout */
    } else if (crm_is_callsite_active(patchset_cs, log_level, 0) == FALSE) {
        return;
    }

    xml_patch_versions(patchset, add, del);
    fmt = crm_element_value(patchset, "format");
    digest = crm_element_value(patchset, XML_ATTR_DIGEST);

    if (add[2] != del[2] || add[1] != del[1] || add[0] != del[0]) {
        do_crm_log_alias(log_level, __FILE__, function, __LINE__,
                         "Diff: --- %d.%d.%d %s", del[0], del[1], del[2], fmt);
        do_crm_log_alias(log_level, __FILE__, function, __LINE__,
                         "Diff: +++ %d.%d.%d %s", add[0], add[1], add[2], digest);

    } else if (patchset != NULL && (add[0] || add[1] || add[2])) {
        do_crm_log_alias(log_level, __FILE__, function, __LINE__, 
                         "%s: Local-only Change: %d.%d.%d", function ? function : "",
                         add[0], add[1], add[2]);
    }

    crm_element_value_int(patchset, "format", &format);
    if(format == 2) {
        xmlNode *change = NULL;

        for (change = __xml_first_child(patchset); change != NULL; change = __xml_next(change)) {
            const char *op = crm_element_value(change, XML_DIFF_OP);
            const char *xpath = crm_element_value(change, XML_DIFF_PATH);

            if(op == NULL) {
            } else if(strcmp(op, "create") == 0) {
                int lpc = 0, max = 0;
                char *prefix = crm_strdup_printf("++ %s: ", xpath);

                max = strlen(prefix);
                __xml_log_element(log_level, __FILE__, function, __LINE__, prefix, change->children,
                                  0, xml_log_option_formatted|xml_log_option_open);

                for(lpc = 2; lpc < max; lpc++) {
                    prefix[lpc] = ' ';
                }

                __xml_log_element(log_level, __FILE__, function, __LINE__, prefix, change->children,
                                  0, xml_log_option_formatted|xml_log_option_close|xml_log_option_children);
                free(prefix);

            } else if(strcmp(op, "move") == 0) {
                do_crm_log_alias(log_level, __FILE__, function, __LINE__, "+~ %s moved to offset %s", xpath, crm_element_value(change, XML_DIFF_POSITION));

            } else if(strcmp(op, "modify") == 0) {
                xmlNode *clist = first_named_child(change, XML_DIFF_LIST);
                char buffer_set[XML_BUFFER_SIZE];
                char buffer_unset[XML_BUFFER_SIZE];
                int o_set = 0;
                int o_unset = 0;

                buffer_set[0] = 0;
                buffer_unset[0] = 0;
                for (child = __xml_first_child(clist); child != NULL; child = __xml_next(child)) {
                    const char *name = crm_element_value(child, "name");

                    op = crm_element_value(child, XML_DIFF_OP);
                    if(op == NULL) {
                    } else if(strcmp(op, "set") == 0) {
                        const char *value = crm_element_value(child, "value");

                        if(o_set > 0) {
                            o_set += snprintf(buffer_set + o_set, XML_BUFFER_SIZE - o_set, ", ");
                        }
                        o_set += snprintf(buffer_set + o_set, XML_BUFFER_SIZE - o_set, "@%s=%s", name, value);

                    } else if(strcmp(op, "unset") == 0) {
                        if(o_unset > 0) {
                            o_unset += snprintf(buffer_unset + o_unset, XML_BUFFER_SIZE - o_unset, ", ");
                        }
                        o_unset += snprintf(buffer_unset + o_unset, XML_BUFFER_SIZE - o_unset, "@%s", name);
                    }
                }
                if(o_set) {
                    do_crm_log_alias(log_level, __FILE__, function, __LINE__, "+  %s:  %s", xpath, buffer_set);
                }
                if(o_unset) {
                    do_crm_log_alias(log_level, __FILE__, function, __LINE__, "-- %s:  %s", xpath, buffer_unset);
                }

            } else if(strcmp(op, "delete") == 0) {
                do_crm_log_alias(log_level, __FILE__, function, __LINE__, "-- %s", xpath);
            }
        }
        return;
    }

    if (log_level < LOG_DEBUG || function == NULL) {
        options |= xml_log_option_diff_short;
    }

    removed = find_xml_node(patchset, "diff-removed", FALSE);
    for (child = __xml_first_child(removed); child != NULL; child = __xml_next(child)) {
        log_data_element(log_level, __FILE__, function, __LINE__, "- ", child, 0,
                         options | xml_log_option_diff_minus);
        if (is_first) {
            is_first = FALSE;
        } else {
            do_crm_log_alias(log_level, __FILE__, function, __LINE__, " --- ");
        }
    }

    is_first = TRUE;
    added = find_xml_node(patchset, "diff-added", FALSE);
    for (child = __xml_first_child(added); child != NULL; child = __xml_next(child)) {
        log_data_element(log_level, __FILE__, function, __LINE__, "+ ", child, 0,
                         options | xml_log_option_diff_plus);
        if (is_first) {
            is_first = FALSE;
        } else {
            do_crm_log_alias(log_level, __FILE__, function, __LINE__, " +++ ");
        }
    }
}
