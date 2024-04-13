pkinit_load_fs_cert_and_key(krb5_context context,
                            pkinit_identity_crypto_context id_cryptoctx,
                            char *certname,
                            char *keyname,
                            int cindex)
{
    krb5_error_code retval;
    X509 *x = NULL;
    EVP_PKEY *y = NULL;

    /* load the certificate */
    retval = get_cert(certname, &x);
    if (retval != 0 || x == NULL) {
        pkiDebug("failed to load user's certificate from '%s'\n", certname);
        goto cleanup;
    }
    retval = get_key(keyname, &y);
    if (retval != 0 || y == NULL) {
        pkiDebug("failed to load user's private key from '%s'\n", keyname);
        goto cleanup;
    }

    id_cryptoctx->creds[cindex] = malloc(sizeof(struct _pkinit_cred_info));
    if (id_cryptoctx->creds[cindex] == NULL) {
        retval = ENOMEM;
        goto cleanup;
    }
    id_cryptoctx->creds[cindex]->name = reassemble_files_name(certname,
                                                              keyname);
    id_cryptoctx->creds[cindex]->cert = x;
#ifndef WITHOUT_PKCS11
    id_cryptoctx->creds[cindex]->cert_id = NULL;
    id_cryptoctx->creds[cindex]->cert_id_len = 0;
#endif
    id_cryptoctx->creds[cindex]->key = y;
    id_cryptoctx->creds[cindex+1] = NULL;

    retval = 0;

cleanup:
    if (retval) {
        if (x != NULL)
            X509_free(x);
        if (y != NULL)
            EVP_PKEY_free(y);
    }
    return retval;
}
