crm_element_value_copy(xmlNode * data, const char *name)
{
    char *value_copy = NULL;
    const char *value = crm_element_value(data, name);

    if (value != NULL) {
        value_copy = strdup(value);
    }
    return value_copy;
}
