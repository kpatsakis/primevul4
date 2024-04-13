pkinit_fini_kdc_req_context(krb5_context context, void *ctx)
{
    pkinit_kdc_req_context reqctx = (pkinit_kdc_req_context)ctx;

    if (reqctx == NULL || reqctx->magic != PKINIT_CTX_MAGIC) {
        pkiDebug("pkinit_fini_kdc_req_context: got bad reqctx (%p)!\n", reqctx);
        return;
    }
    pkiDebug("%s: freeing   reqctx at %p\n", __FUNCTION__, reqctx);

    pkinit_fini_req_crypto(reqctx->cryptoctx);
    if (reqctx->rcv_auth_pack != NULL)
        free_krb5_auth_pack(&reqctx->rcv_auth_pack);
    if (reqctx->rcv_auth_pack9 != NULL)
        free_krb5_auth_pack_draft9(context, &reqctx->rcv_auth_pack9);

    free(reqctx);
}
