hash2smartfield(gpointer key, gpointer value, gpointer user_data)
{
    const char *name = key;
    const char *s_value = value;

    xmlNode *xml_node = user_data;

    if (isdigit(name[0])) {
        xmlNode *tmp = create_xml_node(xml_node, XML_TAG_PARAM);

        crm_xml_add(tmp, XML_NVPAIR_ATTR_NAME, name);
        crm_xml_add(tmp, XML_NVPAIR_ATTR_VALUE, s_value);

    } else if (crm_element_value(xml_node, name) == NULL) {
        crm_xml_add(xml_node, name, s_value);
        crm_trace("dumped: %s=%s", name, s_value);

    } else {
        crm_trace("duplicate: %s=%s", name, s_value);
    }
}
