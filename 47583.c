__get_prefix(const char *prefix, xmlNode *xml, char *buffer, int offset)
{
    const char *id = ID(xml);

    if(offset == 0 && prefix == NULL && xml->parent) {
        offset = __get_prefix(NULL, xml->parent, buffer, offset);
    }

    if(id) {
        offset += snprintf(buffer + offset, XML_BUFFER_SIZE - offset, "/%s[@id='%s']", (const char *)xml->name, id);
    } else if(xml->name) {
        offset += snprintf(buffer + offset, XML_BUFFER_SIZE - offset, "/%s", (const char *)xml->name);
    }

    return offset;
}
