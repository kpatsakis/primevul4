crypto_retrieve_cert_sans(krb5_context context,
                          pkinit_plg_crypto_context plgctx,
                          pkinit_req_crypto_context reqctx,
                          pkinit_identity_crypto_context idctx,
                          krb5_principal **princs_ret,
                          krb5_principal **upn_ret,
                          unsigned char ***dns_ret)
{
    krb5_error_code retval = EINVAL;

    if (reqctx->received_cert == NULL) {
        pkiDebug("%s: No certificate!\n", __FUNCTION__);
        return retval;
    }

    return crypto_retrieve_X509_sans(context, plgctx, reqctx,
                                     reqctx->received_cert, princs_ret,
                                     upn_ret, dns_ret);
}
