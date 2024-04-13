dump_xml_attr(xmlAttrPtr attr, int options, char **buffer, int *offset, int *max)
{
    char *p_value = NULL;
    const char *p_name = NULL;

    CRM_ASSERT(buffer != NULL);
    if (attr == NULL || attr->children == NULL) {
        return;
    }

    p_name = (const char *)attr->name;
    p_value = crm_xml_escape((const char *)attr->children->content);
    buffer_print(*buffer, *max, *offset, " %s=\"%s\"", p_name, p_value);
    free(p_value);
}
