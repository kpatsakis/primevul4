crm_xml_add_last_written(xmlNode *xml_node)
{
    time_t now = time(NULL);
    char *now_str = ctime(&now);

    now_str[24] = EOS; /* replace the newline */
    return crm_xml_add(xml_node, XML_CIB_ATTR_WRITTEN, now_str);
}
