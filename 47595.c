__xml_log_element(int log_level, const char *file, const char *function, int line,
                  const char *prefix, xmlNode * data, int depth, int options)
{
    int max = 0;
    int offset = 0;
    const char *name = NULL;
    const char *hidden = NULL;

    xmlNode *child = NULL;
    xmlAttrPtr pIter = NULL;

    if(data == NULL) {
        return;
    }

    name = crm_element_name(data);

    if(is_set(options, xml_log_option_open)) {
        char *buffer = NULL;

        insert_prefix(options, &buffer, &offset, &max, depth);
        if(data->type == XML_COMMENT_NODE) {
            buffer_print(buffer, max, offset, "<!--");
            buffer_print(buffer, max, offset, "%s", data->content);
            buffer_print(buffer, max, offset, "-->");

        } else {
            buffer_print(buffer, max, offset, "<%s", name);
        }

        hidden = crm_element_value(data, "hidden");
        for (pIter = crm_first_attr(data); pIter != NULL; pIter = pIter->next) {
            xml_private_t *p = pIter->_private;
            const char *p_name = (const char *)pIter->name;
            const char *p_value = crm_attr_value(pIter);
            char *p_copy = NULL;

            if(is_set(p->flags, xpf_deleted)) {
                continue;
            } else if ((is_set(options, xml_log_option_diff_plus)
                 || is_set(options, xml_log_option_diff_minus))
                && strcmp(XML_DIFF_MARKER, p_name) == 0) {
                continue;

            } else if (hidden != NULL && p_name[0] != 0 && strstr(hidden, p_name) != NULL) {
                p_copy = strdup("*****");

            } else {
                p_copy = crm_xml_escape(p_value);
            }

            buffer_print(buffer, max, offset, " %s=\"%s\"", p_name, p_copy);
            free(p_copy);
        }

        if(xml_has_children(data) == FALSE) {
            buffer_print(buffer, max, offset, "/>");

        } else if(is_set(options, xml_log_option_children)) {
            buffer_print(buffer, max, offset, ">");

        } else {
            buffer_print(buffer, max, offset, "/>");
        }

        do_crm_log_alias(log_level, file, function, line, "%s %s", prefix, buffer);
        free(buffer);
    }

    if(data->type == XML_COMMENT_NODE) {
        return;

    } else if(xml_has_children(data) == FALSE) {
        return;

    } else if(is_set(options, xml_log_option_children)) {
        offset = 0;
        max = 0;

        for (child = __xml_first_child(data); child != NULL; child = __xml_next(child)) {
            __xml_log_element(log_level, file, function, line, prefix, child, depth + 1, options|xml_log_option_open|xml_log_option_close);
        }
    }

    if(is_set(options, xml_log_option_close)) {
        char *buffer = NULL;

        insert_prefix(options, &buffer, &offset, &max, depth);
        buffer_print(buffer, max, offset, "</%s>", name);

        do_crm_log_alias(log_level, file, function, line, "%s %s", prefix, buffer);
        free(buffer);
    }
}
