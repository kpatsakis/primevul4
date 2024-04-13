crypto_cert_free_matching_data(krb5_context context,
                               pkinit_cert_matching_data *md)
{
    krb5_principal p;
    int i;

    if (md == NULL)
        return EINVAL;
    if (md->subject_dn)
        free(md->subject_dn);
    if (md->issuer_dn)
        free(md->issuer_dn);
    if (md->sans) {
        for (i = 0, p = md->sans[i]; p != NULL; p = md->sans[++i])
            krb5_free_principal(context, p);
        free(md->sans);
    }
    free(md);
    return 0;
}
