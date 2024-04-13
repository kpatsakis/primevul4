write_xml_fd(xmlNode * xml_node, const char *filename, int fd, gboolean compress)
{
    FILE *stream = NULL;

    CRM_CHECK(fd > 0, return -1);
    stream = fdopen(fd, "w");
    return write_xml_stream(xml_node, filename, stream, compress);
}
