__xml_private_clean(xml_private_t *p)
{
    if(p) {
        CRM_ASSERT(p->check == XML_PRIVATE_MAGIC);

        free(p->user);
        p->user = NULL;

        if(p->acls) {
            g_list_free_full(p->acls, __xml_acl_free);
            p->acls = NULL;
        }

        if(p->deleted_paths) {
            g_list_free_full(p->deleted_paths, free);
            p->deleted_paths = NULL;
        }
    }
}
