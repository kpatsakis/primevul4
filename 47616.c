crm_attr_value(xmlAttr * attr)
{
    if (attr == NULL || attr->children == NULL) {
        return NULL;
    }
    return (const char *)attr->children->content;
}
