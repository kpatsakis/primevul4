kdc_get_ticket_renewtime(kdc_realm_t *realm, krb5_kdc_req *request,
                         krb5_enc_tkt_part *tgt, krb5_db_entry *client,
                         krb5_db_entry *server, krb5_enc_tkt_part *tkt)
{
    krb5_timestamp rtime, max_rlife;

    tkt->times.renew_till = 0;

    /* Don't issue renewable tickets if the client or server don't allow it,
     * or if this is a TGS request and the TGT isn't renewable. */
    if (server->attributes & KRB5_KDB_DISALLOW_RENEWABLE)
        return;
    if (client != NULL && (client->attributes & KRB5_KDB_DISALLOW_RENEWABLE))
        return;
    if (tgt != NULL && !(tgt->flags & TKT_FLG_RENEWABLE))
        return;

    /* Determine the requested renewable time. */
    if (isflagset(request->kdc_options, KDC_OPT_RENEWABLE))
        rtime = request->rtime ? request->rtime : kdc_infinity;
    else if (isflagset(request->kdc_options, KDC_OPT_RENEWABLE_OK) &&
             tkt->times.endtime < request->till)
        rtime = request->till;
    else
        return;

    /* Truncate it to the allowable renewable time. */
    if (tgt != NULL)
        rtime = min(rtime, tgt->times.renew_till);
    max_rlife = min(server->max_renewable_life, realm->realm_maxrlife);
    if (client != NULL)
        max_rlife = min(max_rlife, client->max_renewable_life);
    rtime = min(rtime, tkt->times.starttime + max_rlife);

    /* Make the ticket renewable if the truncated requested time is larger than
     * the ticket end time. */
    if (rtime > tkt->times.endtime) {
        setflag(tkt->flags, TKT_FLG_RENEWABLE);
        tkt->times.renew_till = rtime;
    }
}
