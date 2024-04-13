pkinit_get_kdc_cert(krb5_context context,
                    pkinit_plg_crypto_context plg_cryptoctx,
                    pkinit_req_crypto_context req_cryptoctx,
                    pkinit_identity_crypto_context id_cryptoctx,
                    krb5_principal princ)
{
    krb5_error_code retval = KRB5KDC_ERR_PREAUTH_FAILED;

    req_cryptoctx->received_cert = NULL;
    retval = 0;
    return retval;
}
