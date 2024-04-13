make_toolong_error (void *handle, krb5_data **out)
{
    krb5_error errpkt;
    krb5_error_code retval;
    krb5_data *scratch;
    struct server_handle *h = handle;

    retval = krb5_us_timeofday(h->kdc_err_context,
                               &errpkt.stime, &errpkt.susec);
    if (retval)
        return retval;
    errpkt.error = KRB_ERR_FIELD_TOOLONG;
    errpkt.server = h->kdc_realmlist[0]->realm_tgsprinc;
    errpkt.client = NULL;
    errpkt.cusec = 0;
    errpkt.ctime = 0;
    errpkt.text.length = 0;
    errpkt.text.data = 0;
    errpkt.e_data.length = 0;
    errpkt.e_data.data = 0;
    scratch = malloc(sizeof(*scratch));
    if (scratch == NULL)
        return ENOMEM;
    retval = krb5_mk_error(h->kdc_err_context, &errpkt, scratch);
    if (retval) {
        free(scratch);
        return retval;
    }

    *out = scratch;
    return 0;
}
