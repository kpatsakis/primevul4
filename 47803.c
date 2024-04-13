static int check_revocation(X509_STORE_CTX *ctx)
{
    int i = 0, last = 0, ok = 0;
    if (!(ctx->param->flags & X509_V_FLAG_CRL_CHECK))
        return 1;
    if (ctx->param->flags & X509_V_FLAG_CRL_CHECK_ALL)
        last = sk_X509_num(ctx->chain) - 1;
    else {
        /* If checking CRL paths this isn't the EE certificate */
        if (ctx->parent)
            return 1;
        last = 0;
    }
    for (i = 0; i <= last; i++) {
        ctx->error_depth = i;
        ok = check_cert(ctx);
        if (!ok)
            return ok;
    }
    return 1;
}
