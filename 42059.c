prepare_error_tgs (struct kdc_request_state *state,
                   krb5_kdc_req *request, krb5_ticket *ticket, int error,
                   krb5_principal canon_server,
                   krb5_data **response, const char *status,
                   krb5_pa_data **e_data)
{
    krb5_error errpkt;
    krb5_error_code retval = 0;
    krb5_data *scratch, *e_data_asn1 = NULL, *fast_edata = NULL;

    errpkt.ctime = request->nonce;
    errpkt.cusec = 0;

    if ((retval = krb5_us_timeofday(kdc_context, &errpkt.stime,
                                    &errpkt.susec)))
        return(retval);
    errpkt.error = error;
    errpkt.server = request->server;
    if (ticket && ticket->enc_part2)
        errpkt.client = ticket->enc_part2->client;
    else
        errpkt.client = NULL;
    errpkt.text.length = strlen(status);
    if (!(errpkt.text.data = strdup(status)))
        return ENOMEM;

    if (!(scratch = (krb5_data *)malloc(sizeof(*scratch)))) {
        free(errpkt.text.data);
        return ENOMEM;
    }

    if (e_data != NULL) {
        retval = encode_krb5_padata_sequence(e_data, &e_data_asn1);
        if (retval) {
            free(scratch);
            free(errpkt.text.data);
            return retval;
        }
        errpkt.e_data = *e_data_asn1;
    } else
        errpkt.e_data = empty_data();

    if (state) {
        retval = kdc_fast_handle_error(kdc_context, state, request, e_data,
                                       &errpkt, &fast_edata);
    }
    if (retval) {
        free(scratch);
        free(errpkt.text.data);
        krb5_free_data(kdc_context, e_data_asn1);
        return retval;
    }
    if (fast_edata)
        errpkt.e_data = *fast_edata;
    retval = krb5_mk_error(kdc_context, &errpkt, scratch);
    free(errpkt.text.data);
    krb5_free_data(kdc_context, e_data_asn1);
    krb5_free_data(kdc_context, fast_edata);
    if (retval)
        free(scratch);
    else
        *response = scratch;

    return retval;
}
