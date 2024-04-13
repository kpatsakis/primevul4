dump_pair(gpointer data, gpointer user_data)
{
    name_value_t *pair = data;
    xmlNode *parent = user_data;

    crm_xml_add(parent, pair->name, pair->value);
}
