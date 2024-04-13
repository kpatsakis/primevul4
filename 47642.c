dump_xml_formatted(xmlNode * an_xml_node)
{
    char *buffer = NULL;
    int offset = 0, max = 0;

    crm_xml_dump(an_xml_node, xml_log_option_formatted, &buffer, &offset, &max, 0);
    return buffer;
}
