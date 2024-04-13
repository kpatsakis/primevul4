validate_forwardable(krb5_kdc_req *request, krb5_db_entry client,
                     krb5_db_entry server, krb5_timestamp kdc_time,
                     const char **status)
{
    *status = NULL;
    if (isflagset(request->kdc_options, KDC_OPT_FORWARDABLE) &&
        (isflagset(client.attributes, KRB5_KDB_DISALLOW_FORWARDABLE) ||
         isflagset(server.attributes, KRB5_KDB_DISALLOW_FORWARDABLE))) {
        *status = "FORWARDABLE NOT ALLOWED";
        return(KDC_ERR_POLICY);
    } else
        return 0;
}
