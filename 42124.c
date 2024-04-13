pkinit_get_certs_fs(krb5_context context,
                    pkinit_plg_crypto_context plg_cryptoctx,
                    pkinit_req_crypto_context req_cryptoctx,
                    pkinit_identity_opts *idopts,
                    pkinit_identity_crypto_context id_cryptoctx,
                    krb5_principal princ)
{
    krb5_error_code retval = KRB5KDC_ERR_PREAUTH_FAILED;

    if (idopts->cert_filename == NULL) {
        pkiDebug("%s: failed to get user's cert location\n", __FUNCTION__);
        goto cleanup;
    }

    if (idopts->key_filename == NULL) {
        pkiDebug("%s: failed to get user's private key location\n",
                 __FUNCTION__);
        goto cleanup;
    }

    retval = pkinit_load_fs_cert_and_key(context, id_cryptoctx,
                                         idopts->cert_filename,
                                         idopts->key_filename, 0);
cleanup:
    return retval;
}
