crypto_free_cert_info(krb5_context context,
                      pkinit_plg_crypto_context plg_cryptoctx,
                      pkinit_req_crypto_context req_cryptoctx,
                      pkinit_identity_crypto_context id_cryptoctx)
{
    int i;

    if (id_cryptoctx == NULL)
        return EINVAL;

    for (i = 0; i < MAX_CREDS_ALLOWED; i++) {
        if (id_cryptoctx->creds[i] != NULL) {
            free_cred_info(context, id_cryptoctx, id_cryptoctx->creds[i]);
            id_cryptoctx->creds[i] = NULL;
        }
    }
    return 0;
}
