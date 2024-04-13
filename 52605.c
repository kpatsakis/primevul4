void reset_for_hangup(void *ctx)
{
    int k;
    struct server_handle *h = ctx;

    for (k = 0; k < h->kdc_numrealms; k++)
        krb5_db_refresh_config(h->kdc_realmlist[k]->realm_context);
}
