crypto_cert_iteration_next(krb5_context context,
                           pkinit_cert_iter_handle ih,
                           pkinit_cert_handle *ch_ret)
{
    struct _pkinit_cert_iter_data *id = (struct _pkinit_cert_iter_data *)ih;
    struct _pkinit_cert_data *cd;
    pkinit_identity_crypto_context id_cryptoctx;

    if (id == NULL || id->magic != ITER_MAGIC)
        return EINVAL;

    if (ch_ret == NULL)
        return EINVAL;

    id_cryptoctx = id->idctx;
    if (id_cryptoctx == NULL)
        return EINVAL;

    if (id_cryptoctx->creds[id->index] == NULL)
        return PKINIT_ITER_NO_MORE;

    cd = calloc(1, sizeof(*cd));
    if (cd == NULL)
        return ENOMEM;

    cd->magic = CERT_MAGIC;
    cd->plgctx = id->plgctx;
    cd->reqctx = id->reqctx;
    cd->idctx = id->idctx;
    cd->index = id->index;
    cd->cred = id_cryptoctx->creds[id->index++];
    *ch_ret = (pkinit_cert_handle)cd;
    return 0;
}
