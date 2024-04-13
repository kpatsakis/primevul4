crypto_cert_get_count(krb5_context context,
                      pkinit_plg_crypto_context plg_cryptoctx,
                      pkinit_req_crypto_context req_cryptoctx,
                      pkinit_identity_crypto_context id_cryptoctx,
                      int *cert_count)
{
    int count;

    if (id_cryptoctx == NULL || id_cryptoctx->creds[0] == NULL)
        return EINVAL;

    for (count = 0;
         count <= MAX_CREDS_ALLOWED && id_cryptoctx->creds[count] != NULL;
         count++);
    *cert_count = count;
    return 0;
}
