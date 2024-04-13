xml_get_path(xmlNode *xml)
{
    int offset = 0;
    char buffer[XML_BUFFER_SIZE];

    if(__get_prefix(NULL, xml, buffer, offset) > 0) {
        return strdup(buffer);
    }
    return NULL;
}
