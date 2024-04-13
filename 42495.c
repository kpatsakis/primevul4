pkinit_server_plugin_fini(krb5_context context,
                          krb5_kdcpreauth_moddata moddata)
{
    pkinit_kdc_context *realm_contexts = (pkinit_kdc_context *)moddata;
    int i;

    if (realm_contexts == NULL)
        return;

    for (i = 0; realm_contexts[i] != NULL; i++) {
        pkinit_server_plugin_fini_realm(context, realm_contexts[i]);
    }
    pkiDebug("%s: freeing   context at %p\n", __FUNCTION__, realm_contexts);
    free(realm_contexts);
}
