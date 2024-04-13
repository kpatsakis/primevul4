crm_first_attr(xmlNode * xml)
{
    if (xml == NULL) {
        return NULL;
    }
    return xml->properties;
}
