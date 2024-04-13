__xml_log_change_element(int log_level, const char *file, const char *function, int line,
                         const char *prefix, xmlNode * data, int depth, int options)
{
    xml_private_t *p;
    char *prefix_m = NULL;
    xmlNode *child = NULL;
    xmlAttrPtr pIter = NULL;

    if(data == NULL) {
        return;
    }

    p = data->_private;

    prefix_m = strdup(prefix);
    prefix_m[1] = '+';

    if(is_set(p->flags, xpf_dirty) && is_set(p->flags, xpf_created)) {
        /* Continue and log full subtree */
        __xml_log_element(log_level, file, function, line,
                          prefix_m, data, depth, options|xml_log_option_open|xml_log_option_close|xml_log_option_children);

    } else if(is_set(p->flags, xpf_dirty)) {
        char *spaces = calloc(80, 1);
        int s_count = 0, s_max = 80;
        char *prefix_del = NULL;
        char *prefix_moved = NULL;
        const char *flags = prefix;

        insert_prefix(options, &spaces, &s_count, &s_max, depth);
        prefix_del = strdup(prefix);
        prefix_del[0] = '-';
        prefix_del[1] = '-';
        prefix_moved = strdup(prefix);
        prefix_moved[1] = '~';

        if(is_set(p->flags, xpf_moved)) {
            flags = prefix_moved;
        } else {
            flags = prefix;
        }

        __xml_log_element(log_level, file, function, line,
                          flags, data, depth, options|xml_log_option_open);

        for (pIter = crm_first_attr(data); pIter != NULL; pIter = pIter->next) {
            const char *aname = (const char*)pIter->name;

            p = pIter->_private;
            if(is_set(p->flags, xpf_deleted)) {
                const char *value = crm_element_value(data, aname);
                flags = prefix_del;
                do_crm_log_alias(log_level, file, function, line,
                                 "%s %s @%s=%s", flags, spaces, aname, value);

            } else if(is_set(p->flags, xpf_dirty)) {
                const char *value = crm_element_value(data, aname);

                if(is_set(p->flags, xpf_created)) {
                    flags = prefix_m;

                } else if(is_set(p->flags, xpf_modified)) {
                    flags = prefix;

                } else if(is_set(p->flags, xpf_moved)) {
                    flags = prefix_moved;

                } else {
                    flags = prefix;
                }
                do_crm_log_alias(log_level, file, function, line,
                                 "%s %s @%s=%s", flags, spaces, aname, value);
            }
        }
        free(prefix_moved);
        free(prefix_del);
        free(spaces);

        for (child = __xml_first_child(data); child != NULL; child = __xml_next(child)) {
            __xml_log_change_element(log_level, file, function, line, prefix, child, depth + 1, options);
        }

        __xml_log_element(log_level, file, function, line,
                          prefix, data, depth, options|xml_log_option_close);

    } else {
        for (child = __xml_first_child(data); child != NULL; child = __xml_next(child)) {
            __xml_log_change_element(log_level, file, function, line, prefix, child, depth + 1, options);
        }
    }

    free(prefix_m);

}
