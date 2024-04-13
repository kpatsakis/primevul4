static int check_issued(X509_STORE_CTX *ctx, X509 *x, X509 *issuer)
{
    int ret;
    if (x == issuer)
        return cert_self_signed(x);
    ret = X509_check_issued(issuer, x);
    if (ret == X509_V_OK) {
        int i;
        X509 *ch;
        /* Special case: single self signed certificate */
        if (cert_self_signed(x) && sk_X509_num(ctx->chain) == 1)
            return 1;
        for (i = 0; i < sk_X509_num(ctx->chain); i++) {
            ch = sk_X509_value(ctx->chain, i);
            if (ch == issuer || !X509_cmp(ch, issuer)) {
                ret = X509_V_ERR_PATH_LOOP;
                break;
            }
        }
    }

    if (ret == X509_V_OK)
        return 1;
    /* If we haven't asked for issuer errors don't set ctx */
    if (!(ctx->param->flags & X509_V_FLAG_CB_ISSUER_CHECK))
        return 0;

    ctx->error = ret;
    ctx->current_cert = x;
    ctx->current_issuer = issuer;
    return ctx->verify_cb(0, ctx);
}
