add_message_xml(xmlNode * msg, const char *field, xmlNode * xml)
{
    xmlNode *holder = create_xml_node(msg, field);

    add_node_copy(holder, xml);
    return TRUE;
}
