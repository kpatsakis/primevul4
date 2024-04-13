int X509_STORE_CTX_init(X509_STORE_CTX *ctx, X509_STORE *store, X509 *x509,
                        STACK_OF(X509) *chain)
{
    int ret = 1;
    ctx->ctx = store;
    ctx->current_method = 0;
    ctx->cert = x509;
    ctx->untrusted = chain;
    ctx->crls = NULL;
    ctx->last_untrusted = 0;
    ctx->other_ctx = NULL;
    ctx->valid = 0;
    ctx->chain = NULL;
    ctx->error = 0;
    ctx->explicit_policy = 0;
    ctx->error_depth = 0;
    ctx->current_cert = NULL;
    ctx->current_issuer = NULL;
    ctx->current_crl = NULL;
    ctx->current_crl_score = 0;
    ctx->current_reasons = 0;
    ctx->tree = NULL;
    ctx->parent = NULL;

    ctx->param = X509_VERIFY_PARAM_new();

    if (!ctx->param) {
        X509err(X509_F_X509_STORE_CTX_INIT, ERR_R_MALLOC_FAILURE);
        return 0;
    }

    /*
     * Inherit callbacks and flags from X509_STORE if not set use defaults.
     */

    if (store)
        ret = X509_VERIFY_PARAM_inherit(ctx->param, store->param);
    else
        ctx->param->inh_flags |= X509_VP_FLAG_DEFAULT | X509_VP_FLAG_ONCE;

    if (store) {
        ctx->verify_cb = store->verify_cb;
        ctx->cleanup = store->cleanup;
    } else
        ctx->cleanup = 0;

    if (ret)
        ret = X509_VERIFY_PARAM_inherit(ctx->param,
                                        X509_VERIFY_PARAM_lookup("default"));

    if (ret == 0) {
        X509err(X509_F_X509_STORE_CTX_INIT, ERR_R_MALLOC_FAILURE);
        return 0;
    }

    if (store && store->check_issued)
        ctx->check_issued = store->check_issued;
    else
        ctx->check_issued = check_issued;

    if (store && store->get_issuer)
        ctx->get_issuer = store->get_issuer;
    else
        ctx->get_issuer = X509_STORE_CTX_get1_issuer;

    if (store && store->verify_cb)
        ctx->verify_cb = store->verify_cb;
    else
        ctx->verify_cb = null_callback;

    if (store && store->verify)
        ctx->verify = store->verify;
    else
        ctx->verify = internal_verify;

    if (store && store->check_revocation)
        ctx->check_revocation = store->check_revocation;
    else
        ctx->check_revocation = check_revocation;

    if (store && store->get_crl)
        ctx->get_crl = store->get_crl;
    else
        ctx->get_crl = NULL;

    if (store && store->check_crl)
        ctx->check_crl = store->check_crl;
    else
        ctx->check_crl = check_crl;

    if (store && store->cert_crl)
        ctx->cert_crl = store->cert_crl;
    else
        ctx->cert_crl = cert_crl;

    if (store && store->lookup_certs)
        ctx->lookup_certs = store->lookup_certs;
    else
        ctx->lookup_certs = X509_STORE_get1_certs;

    if (store && store->lookup_crls)
        ctx->lookup_crls = store->lookup_crls;
    else
        ctx->lookup_crls = X509_STORE_get1_crls;

    ctx->check_policy = check_policy;

    /*
     * Since X509_STORE_CTX_cleanup does a proper "free" on the ex_data, we
     * put a corresponding "new" here.
     */
    if (!CRYPTO_new_ex_data(CRYPTO_EX_INDEX_X509_STORE_CTX, ctx,
                            &(ctx->ex_data))) {
        OPENSSL_free(ctx);
        X509err(X509_F_X509_STORE_CTX_INIT, ERR_R_MALLOC_FAILURE);
        return 0;
    }
    return 1;
}
