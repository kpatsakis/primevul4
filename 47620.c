crm_element_value_const_int(const xmlNode * data, const char *name, int *dest)
{
    return crm_element_value_int((xmlNode *) data, name, dest);
}
