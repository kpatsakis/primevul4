static X509 *find_issuer(X509_STORE_CTX *ctx, STACK_OF(X509) *sk, X509 *x)
{
    int i;
    X509 *issuer, *rv = NULL;;
    for (i = 0; i < sk_X509_num(sk); i++) {
        issuer = sk_X509_value(sk, i);
        if (ctx->check_issued(ctx, x, issuer)) {
            rv = issuer;
            if (x509_check_cert_time(ctx, rv, 1))
                break;
        }
    }
    return rv;
}
