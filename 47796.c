X509_STORE_CTX *X509_STORE_CTX_new(void)
{
    X509_STORE_CTX *ctx = OPENSSL_malloc(sizeof(*ctx));

    if (!ctx) {
        X509err(X509_F_X509_STORE_CTX_NEW, ERR_R_MALLOC_FAILURE);
        return NULL;
    }
    memset(ctx, 0, sizeof(*ctx));
    return ctx;
}
