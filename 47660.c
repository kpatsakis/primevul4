get_xpath_object_relative(const char *xpath, xmlNode * xml_obj, int error_level)
{
    int len = 0;
    xmlNode *result = NULL;
    char *xpath_full = NULL;
    char *xpath_prefix = NULL;

    if (xml_obj == NULL || xpath == NULL) {
        return NULL;
    }

    xpath_prefix = (char *)xmlGetNodePath(xml_obj);
    len += strlen(xpath_prefix);
    len += strlen(xpath);

    xpath_full = strdup(xpath_prefix);
    xpath_full = realloc_safe(xpath_full, len + 1);
    strncat(xpath_full, xpath, len);

    result = get_xpath_object(xpath_full, xml_obj, error_level);

    free(xpath_prefix);
    free(xpath_full);
    return result;
}
