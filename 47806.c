static int internal_verify(X509_STORE_CTX *ctx)
{
    int ok = 0, n;
    X509 *xs, *xi;
    EVP_PKEY *pkey = NULL;
    int (*cb) (int xok, X509_STORE_CTX *xctx);

    cb = ctx->verify_cb;

    n = sk_X509_num(ctx->chain);
    ctx->error_depth = n - 1;
    n--;
    xi = sk_X509_value(ctx->chain, n);

    if (ctx->check_issued(ctx, xi, xi))
        xs = xi;
    else {
        if (ctx->param->flags & X509_V_FLAG_PARTIAL_CHAIN) {
            xs = xi;
            goto check_cert;
        }
        if (n <= 0) {
            ctx->error = X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE;
            ctx->current_cert = xi;
            ok = cb(0, ctx);
            goto end;
        } else {
            n--;
            ctx->error_depth = n;
            xs = sk_X509_value(ctx->chain, n);
        }
    }

/*      ctx->error=0;  not needed */
    while (n >= 0) {
        ctx->error_depth = n;

        /*
         * Skip signature check for self signed certificates unless
         * explicitly asked for. It doesn't add any security and just wastes
         * time.
         */
        if (!xs->valid
            && (xs != xi
                || (ctx->param->flags & X509_V_FLAG_CHECK_SS_SIGNATURE))) {
            if ((pkey = X509_get_pubkey(xi)) == NULL) {
                ctx->error = X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY;
                ctx->current_cert = xi;
                ok = (*cb) (0, ctx);
                if (!ok)
                    goto end;
            } else if (X509_verify(xs, pkey) <= 0) {
                ctx->error = X509_V_ERR_CERT_SIGNATURE_FAILURE;
                ctx->current_cert = xs;
                ok = (*cb) (0, ctx);
                if (!ok) {
                    EVP_PKEY_free(pkey);
                    goto end;
                }
            }
            EVP_PKEY_free(pkey);
            pkey = NULL;
        }

        xs->valid = 1;

 check_cert:
        ok = x509_check_cert_time(ctx, xs, 0);
        if (!ok)
            goto end;

        /* The last error (if any) is still in the error value */
        ctx->current_issuer = xi;
        ctx->current_cert = xs;
        ok = (*cb) (1, ctx);
        if (!ok)
            goto end;

        n--;
        if (n >= 0) {
            xi = xs;
            xs = sk_X509_value(ctx->chain, n);
        }
    }
    ok = 1;
 end:
    return ok;
}
