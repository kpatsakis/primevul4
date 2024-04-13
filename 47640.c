dump_xml_comment(xmlNode * data, int options, char **buffer, int *offset, int *max, int depth)
{
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

    insert_prefix(options, buffer, offset, max, depth);

    buffer_print(*buffer, *max, *offset, "<!--");
    buffer_print(*buffer, *max, *offset, "%s", data->content);
    buffer_print(*buffer, *max, *offset, "-->");

    if (options & xml_log_option_formatted) {
        buffer_print(*buffer, *max, *offset, "\n");
    }
}
