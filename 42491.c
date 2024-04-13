pkinit_init_kdc_req_context(krb5_context context, pkinit_kdc_req_context *ctx)
{
    krb5_error_code retval = ENOMEM;
    pkinit_kdc_req_context reqctx = NULL;

    reqctx = malloc(sizeof(*reqctx));
    if (reqctx == NULL)
        return retval;
    memset(reqctx, 0, sizeof(*reqctx));
    reqctx->magic = PKINIT_CTX_MAGIC;

    retval = pkinit_init_req_crypto(&reqctx->cryptoctx);
    if (retval)
        goto cleanup;
    reqctx->rcv_auth_pack = NULL;
    reqctx->rcv_auth_pack9 = NULL;

    pkiDebug("%s: returning reqctx at %p\n", __FUNCTION__, reqctx);
    *ctx = reqctx;
    retval = 0;
cleanup:
    if (retval)
        pkinit_fini_kdc_req_context(context, reqctx);

    return retval;
}
