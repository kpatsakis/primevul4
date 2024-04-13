dump_xml_unformatted(xmlNode * an_xml_node)
{
    char *buffer = NULL;
    int offset = 0, max = 0;

    crm_xml_dump(an_xml_node, 0, &buffer, &offset, &max, 0);
    return buffer;
}
