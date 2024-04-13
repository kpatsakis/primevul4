crm_xml_add_int(xmlNode * node, const char *name, int value)
{
    char *number = crm_itoa(value);
    const char *added = crm_xml_add(node, name, number);

    free(number);
    return added;
}
