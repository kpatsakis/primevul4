hash2field(gpointer key, gpointer value, gpointer user_data)
{
    const char *name = key;
    const char *s_value = value;

    xmlNode *xml_node = user_data;

    if (crm_element_value(xml_node, name) == NULL) {
        crm_xml_add(xml_node, name, s_value);

    } else {
        crm_trace("duplicate: %s=%s", name, s_value);
    }
}
