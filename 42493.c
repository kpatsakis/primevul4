pkinit_server_get_edata(krb5_context context,
                        krb5_kdc_req *request,
                        krb5_kdcpreauth_callbacks cb,
                        krb5_kdcpreauth_rock rock,
                        krb5_kdcpreauth_moddata moddata,
                        krb5_preauthtype pa_type,
                        krb5_kdcpreauth_edata_respond_fn respond,
                        void *arg)
{
    krb5_error_code retval = 0;
    pkinit_kdc_context plgctx = NULL;

    pkiDebug("pkinit_server_get_edata: entered!\n");


    /*
     * If we don't have a realm context for the given realm,
     * don't tell the client that we support pkinit!
     */
    plgctx = pkinit_find_realm_context(context, moddata, request->server);
    if (plgctx == NULL)
        retval = EINVAL;

    (*respond)(arg, retval, NULL);
}
