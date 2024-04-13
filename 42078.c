crypto_cert_iteration_begin(krb5_context context,
                            pkinit_plg_crypto_context plg_cryptoctx,
                            pkinit_req_crypto_context req_cryptoctx,
                            pkinit_identity_crypto_context id_cryptoctx,
                            pkinit_cert_iter_handle *ih_ret)
{
    struct _pkinit_cert_iter_data *id;

    if (id_cryptoctx == NULL || ih_ret == NULL)
        return EINVAL;
    if (id_cryptoctx->creds[0] == NULL) /* No cred info available */
        return ENOENT;

    id = calloc(1, sizeof(*id));
    if (id == NULL)
        return ENOMEM;
    id->magic = ITER_MAGIC;
    id->plgctx = plg_cryptoctx,
        id->reqctx = req_cryptoctx,
        id->idctx = id_cryptoctx;
    id->index = 0;
    *ih_ret = (pkinit_cert_iter_handle) id;
    return 0;
}
