dump_xml_element(xmlNode * data, int options, char **buffer, int *offset, int *max, int depth)
{
    const char *name = NULL;

    CRM_ASSERT(max != NULL);
    CRM_ASSERT(offset != NULL);
    CRM_ASSERT(buffer != NULL);

    if (data == NULL) {
        crm_trace("Nothing to dump");
        return;
    }

    if (*buffer == NULL) {
        *offset = 0;
        *max = 0;
    }

    name = crm_element_name(data);
    CRM_ASSERT(name != NULL);

    insert_prefix(options, buffer, offset, max, depth);
    buffer_print(*buffer, *max, *offset, "<%s", name);

    if (options & xml_log_option_filtered) {
        dump_filtered_xml(data, options, buffer, offset, max);

    } else {
        xmlAttrPtr xIter = NULL;

        for (xIter = crm_first_attr(data); xIter != NULL; xIter = xIter->next) {
            dump_xml_attr(xIter, options, buffer, offset, max);
        }
    }

    if (data->children == NULL) {
        buffer_print(*buffer, *max, *offset, "/>");

    } else {
        buffer_print(*buffer, *max, *offset, ">");
    }

    if (options & xml_log_option_formatted) {
        buffer_print(*buffer, *max, *offset, "\n");
    }

    if (data->children) {
        xmlNode *xChild = NULL;

        for (xChild = __xml_first_child(data); xChild != NULL; xChild = __xml_next(xChild)) {
            crm_xml_dump(xChild, options, buffer, offset, max, depth + 1);
        }

        insert_prefix(options, buffer, offset, max, depth);
        buffer_print(*buffer, *max, *offset, "</%s>", name);

        if (options & xml_log_option_formatted) {
            buffer_print(*buffer, *max, *offset, "\n");
        }
    }
}
