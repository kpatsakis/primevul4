create_krb5_trustedCertifiers(krb5_context context,
                              pkinit_plg_crypto_context plg_cryptoctx,
                              pkinit_req_crypto_context req_cryptoctx,
                              pkinit_identity_crypto_context id_cryptoctx,
                              krb5_external_principal_identifier *** ids)
{

    krb5_error_code retval = ENOMEM;
    STACK_OF(X509) *sk = id_cryptoctx->trustedCAs;

    *ids = NULL;
    if (id_cryptoctx->trustedCAs == NULL)
        return KRB5KDC_ERR_PREAUTH_FAILED;

    retval = create_identifiers_from_stack(sk, ids);

    return retval;
}
