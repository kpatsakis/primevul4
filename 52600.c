kdc_rd_ap_req(kdc_realm_t *kdc_active_realm,
              krb5_ap_req *apreq, krb5_auth_context auth_context,
              krb5_db_entry **server, krb5_keyblock **tgskey)
{
    krb5_error_code     retval;
    krb5_enctype        search_enctype = apreq->ticket->enc_part.enctype;
    krb5_boolean        match_enctype = 1;
    krb5_kvno           kvno;
    size_t              tries = 3;

    /*
     * When we issue tickets we use the first key in the principals' highest
     * kvno keyset.  For non-cross-realm krbtgt principals we want to only
     * allow the use of the first key of the principal's keyset that matches
     * the given kvno.
     */
    if (krb5_is_tgs_principal(apreq->ticket->server) &&
        !is_cross_tgs_principal(apreq->ticket->server)) {
        search_enctype = -1;
        match_enctype = 0;
    }

    retval = kdc_get_server_key(kdc_context, apreq->ticket,
                                KRB5_KDB_FLAG_ALIAS_OK, match_enctype, server,
                                NULL, NULL);
    if (retval)
        return retval;

    *tgskey = NULL;
    kvno = apreq->ticket->enc_part.kvno;
    do {
        krb5_free_keyblock(kdc_context, *tgskey);
        retval = find_server_key(kdc_context,
                                 *server, search_enctype, kvno, tgskey, &kvno);
        if (retval)
            continue;

        /* Make the TGS key available to krb5_rd_req_decoded_anyflag() */
        retval = krb5_auth_con_setuseruserkey(kdc_context, auth_context,
                                              *tgskey);
        if (retval)
            return retval;

        retval = krb5_rd_req_decoded_anyflag(kdc_context, &auth_context, apreq,
                                             apreq->ticket->server,
                                             kdc_active_realm->realm_keytab,
                                             NULL, NULL);

        /* If the ticket was decrypted, don't try any more keys. */
        if (apreq->ticket->enc_part2 != NULL)
            break;

    } while (retval && apreq->ticket->enc_part.kvno == 0 && kvno-- > 1 &&
             --tries > 0);

    return retval;
}
