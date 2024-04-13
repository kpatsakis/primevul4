pkinit_find_realm_context(krb5_context context,
                          krb5_kdcpreauth_moddata moddata,
                          krb5_principal princ)
{
    int i;
    pkinit_kdc_context *realm_contexts = (pkinit_kdc_context *)moddata;

    if (moddata == NULL)
        return NULL;

    for (i = 0; realm_contexts[i] != NULL; i++) {
        pkinit_kdc_context p = realm_contexts[i];

        if ((p->realmname_len == princ->realm.length) &&
            (strncmp(p->realmname, princ->realm.data, p->realmname_len) == 0)) {
            pkiDebug("%s: returning context at %p for realm '%s'\n",
                     __FUNCTION__, p, p->realmname);
            return p;
        }
    }
    pkiDebug("%s: unable to find realm context for realm '%.*s'\n",
             __FUNCTION__, princ->realm.length, princ->realm.data);
    return NULL;
}
