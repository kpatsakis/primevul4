write_xml_file(xmlNode * xml_node, const char *filename, gboolean compress)
{
    FILE *stream = NULL;

    stream = fopen(filename, "w");

    return write_xml_stream(xml_node, filename, stream, compress);
}
