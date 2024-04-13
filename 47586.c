__xml_acl_free(void *data)
{
    if(data) {
        xml_acl_t *acl = data;

        free(acl->xpath);
        free(acl);
    }
}
