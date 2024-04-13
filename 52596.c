kdc_get_ticket_endtime(kdc_realm_t *kdc_active_realm,
                       krb5_timestamp starttime,
                       krb5_timestamp endtime,
                       krb5_timestamp till,
                       krb5_db_entry *client,
                       krb5_db_entry *server,
                       krb5_timestamp *out_endtime)
{
    krb5_timestamp until, life;

    if (till == 0)
        till = kdc_infinity;

    until = min(till, endtime);

    life = until - starttime;

    if (client != NULL && client->max_life != 0)
        life = min(life, client->max_life);
    if (server->max_life != 0)
        life = min(life, server->max_life);
    if (kdc_active_realm->realm_maxlife != 0)
        life = min(life, kdc_active_realm->realm_maxlife);

    *out_endtime = starttime + life;
}
