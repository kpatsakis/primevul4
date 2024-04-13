validate_transit_path(krb5_context context,
                      krb5_const_principal client,
                      krb5_db_entry *server,
                      krb5_db_entry *header_srv)
{
    /* Incoming */
    if (isflagset(server->attributes, KRB5_KDB_XREALM_NON_TRANSITIVE)) {
        return KRB5KDC_ERR_PATH_NOT_ACCEPTED;
    }

    /* Outgoing */
    if (isflagset(header_srv->attributes, KRB5_KDB_XREALM_NON_TRANSITIVE) &&
        (!krb5_principal_compare(context, server->princ, header_srv->princ) ||
         !krb5_realm_compare(context, client, header_srv->princ))) {
        return KRB5KDC_ERR_PATH_NOT_ACCEPTED;
    }

    return 0;
}
