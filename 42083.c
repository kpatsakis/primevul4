crypto_cert_select_default(krb5_context context,
                           pkinit_plg_crypto_context plg_cryptoctx,
                           pkinit_req_crypto_context req_cryptoctx,
                           pkinit_identity_crypto_context id_cryptoctx)
{
    krb5_error_code retval;
    int cert_count = 0;

    retval = crypto_cert_get_count(context, plg_cryptoctx, req_cryptoctx,
                                   id_cryptoctx, &cert_count);
    if (retval) {
        pkiDebug("%s: crypto_cert_get_count error %d, %s\n",
                 __FUNCTION__, retval, error_message(retval));
        goto errout;
    }
    if (cert_count != 1) {
        pkiDebug("%s: ERROR: There are %d certs to choose from, "
                 "but there must be exactly one.\n",
                 __FUNCTION__, cert_count);
        retval = EINVAL;
        goto errout;
    }
    /* copy the selected cert into our id_cryptoctx */
    if (id_cryptoctx->my_certs != NULL) {
        sk_X509_pop_free(id_cryptoctx->my_certs, X509_free);
    }
    id_cryptoctx->my_certs = sk_X509_new_null();
    sk_X509_push(id_cryptoctx->my_certs, id_cryptoctx->creds[0]->cert);
    id_cryptoctx->creds[0]->cert = NULL;        /* Don't free it twice */
    id_cryptoctx->cert_index = 0;
    /* hang on to the selected credential name */
    if (id_cryptoctx->creds[0]->name != NULL)
        id_cryptoctx->identity = strdup(id_cryptoctx->creds[0]->name);
    else
        id_cryptoctx->identity = NULL;

    if (id_cryptoctx->pkcs11_method != 1) {
        id_cryptoctx->my_key = id_cryptoctx->creds[0]->key;
        id_cryptoctx->creds[0]->key = NULL;     /* Don't free it twice */
    }
#ifndef WITHOUT_PKCS11
    else {
        id_cryptoctx->cert_id = id_cryptoctx->creds[0]->cert_id;
        id_cryptoctx->creds[0]->cert_id = NULL; /* Don't free it twice */
        id_cryptoctx->cert_id_len = id_cryptoctx->creds[0]->cert_id_len;
    }
#endif
    retval = 0;
errout:
    return retval;
}
