stdin2xml(void)
{
    size_t data_length = 0;
    size_t read_chars = 0;

    char *xml_buffer = NULL;
    xmlNode *xml_obj = NULL;

    do {
        size_t next = XML_BUFFER_SIZE + data_length + 1;

        if(next <= 0) {
            crm_err("Buffer size exceeded at: %l + %d", data_length, XML_BUFFER_SIZE);
            break;
        }

        xml_buffer = realloc_safe(xml_buffer, next);
        read_chars = fread(xml_buffer + data_length, 1, XML_BUFFER_SIZE, stdin);
        data_length += read_chars;
    } while (read_chars > 0);

    if (data_length == 0) {
        crm_warn("No XML supplied on stdin");
        free(xml_buffer);
        return NULL;
    }

    xml_buffer[data_length] = '\0';

    xml_obj = string2xml(xml_buffer);
    free(xml_buffer);

    crm_log_xml_trace(xml_obj, "Created fragment");
    return xml_obj;
}
