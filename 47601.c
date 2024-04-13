__xml_private_free(xml_private_t *p)
{
    __xml_private_clean(p);
    free(p);
}
