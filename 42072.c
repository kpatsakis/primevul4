create_krb5_invalidCertificates(krb5_context context,
                                pkinit_plg_crypto_context plg_cryptoctx,
                                pkinit_req_crypto_context req_cryptoctx,
                                pkinit_identity_crypto_context id_cryptoctx,
                                krb5_external_principal_identifier *** ids)
{

    krb5_error_code retval = ENOMEM;
    STACK_OF(X509) *sk = NULL;

    *ids = NULL;
    if (req_cryptoctx->received_cert == NULL)
        return KRB5KDC_ERR_PREAUTH_FAILED;

    sk = sk_X509_new_null();
    if (sk == NULL)
        goto cleanup;
    sk_X509_push(sk, req_cryptoctx->received_cert);

    retval = create_identifiers_from_stack(sk, ids);

    sk_X509_free(sk);
cleanup:

    return retval;
}
