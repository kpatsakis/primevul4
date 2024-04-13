crm_element_value_int(xmlNode * data, const char *name, int *dest)
{
    const char *value = crm_element_value(data, name);

    CRM_CHECK(dest != NULL, return -1);
    if (value) {
        *dest = crm_int_helper(value, NULL);
        return 0;
    }
    return -1;
}
