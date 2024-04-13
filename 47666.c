log_data_element(int log_level, const char *file, const char *function, int line,
                 const char *prefix, xmlNode * data, int depth, int options)
{
    xmlNode *a_child = NULL;

    char *prefix_m = NULL;

    if (prefix == NULL) {
        prefix = "";
    }

    /* Since we use the same file and line, to avoid confusing libqb, we need to use the same format strings */
    if (data == NULL) {
        do_crm_log_alias(log_level, file, function, line, "%s: %s", prefix,
                         "No data to dump as XML");
        return;
    }

    if(is_set(options, xml_log_option_dirty_add) || is_set(options, xml_log_option_dirty_add)) {
        __xml_log_change_element(log_level, file, function, line, prefix, data, depth, options);
        return;
    }

    if (is_set(options, xml_log_option_formatted)) {
        if (is_set(options, xml_log_option_diff_plus)
            && (data->children == NULL || crm_element_value(data, XML_DIFF_MARKER))) {
            options |= xml_log_option_diff_all;
            prefix_m = strdup(prefix);
            prefix_m[1] = '+';
            prefix = prefix_m;

        } else if (is_set(options, xml_log_option_diff_minus)
                   && (data->children == NULL || crm_element_value(data, XML_DIFF_MARKER))) {
            options |= xml_log_option_diff_all;
            prefix_m = strdup(prefix);
            prefix_m[1] = '-';
            prefix = prefix_m;
        }
    }

    if (is_set(options, xml_log_option_diff_short)
               && is_not_set(options, xml_log_option_diff_all)) {
        /* Still searching for the actual change */
        for (a_child = __xml_first_child(data); a_child != NULL; a_child = __xml_next(a_child)) {
            log_data_element(log_level, file, function, line, prefix, a_child, depth + 1, options);
        }
        return;
    }

    __xml_log_element(log_level, file, function, line,
                      prefix, data, depth, options|xml_log_option_open|xml_log_option_close|xml_log_option_children);
    free(prefix_m);
}
