pkinit_create_td_invalid_certificate(
    krb5_context context,
    pkinit_plg_crypto_context plg_cryptoctx,
    pkinit_req_crypto_context req_cryptoctx,
    pkinit_identity_crypto_context id_cryptoctx,
    krb5_pa_data ***e_data_out)
{
    krb5_error_code retval = KRB5KRB_ERR_GENERIC;

    retval = pkinit_create_sequence_of_principal_identifiers(context,
                                                             plg_cryptoctx, req_cryptoctx, id_cryptoctx,
                                                             TD_INVALID_CERTIFICATES, e_data_out);

    return retval;
}
