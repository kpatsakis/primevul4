set_salt_padata(METHOD_DATA *md, Salt *salt)
{
    if (salt) {
       realloc_method_data(md);
       md->val[md->len - 1].padata_type = salt->type;
       der_copy_octet_string(&salt->salt,
                             &md->val[md->len - 1].padata_value);
    }
}
