kdc_get_server_key(krb5_context context,
                   krb5_ticket *ticket, unsigned int flags,
                   krb5_boolean match_enctype, krb5_db_entry **server_ptr,
                   krb5_keyblock **key, krb5_kvno *kvno)
{
    krb5_error_code       retval;
    krb5_db_entry       * server = NULL;
    krb5_enctype          search_enctype = -1;
    krb5_kvno             search_kvno = -1;

    if (match_enctype)
        search_enctype = ticket->enc_part.enctype;
    if (ticket->enc_part.kvno)
        search_kvno = ticket->enc_part.kvno;

    *server_ptr = NULL;

    retval = krb5_db_get_principal(context, ticket->server, flags,
                                   &server);
    if (retval == KRB5_KDB_NOENTRY) {
        char *sname;
        if (!krb5_unparse_name(context, ticket->server, &sname)) {
            limit_string(sname);
            krb5_klog_syslog(LOG_ERR,
                             _("TGS_REQ: UNKNOWN SERVER: server='%s'"), sname);
            free(sname);
        }
        return KRB5KDC_ERR_S_PRINCIPAL_UNKNOWN;
    } else if (retval)
        return retval;
    if (server->attributes & KRB5_KDB_DISALLOW_SVR ||
        server->attributes & KRB5_KDB_DISALLOW_ALL_TIX) {
        retval = KRB5KDC_ERR_S_PRINCIPAL_UNKNOWN;
        goto errout;
    }

    if (key) {
        retval = find_server_key(context, server, search_enctype, search_kvno,
                                 key, kvno);
        if (retval)
            goto errout;
    }
    *server_ptr = server;
    server = NULL;
    return 0;

errout:
    krb5_db_free_principal(context, server);
    return retval;
}
