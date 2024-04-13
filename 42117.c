pkinit_fini_certs(pkinit_identity_crypto_context ctx)
{
    if (ctx == NULL)
        return;

    if (ctx->my_certs != NULL)
        sk_X509_pop_free(ctx->my_certs, X509_free);

    if (ctx->my_key != NULL)
        EVP_PKEY_free(ctx->my_key);

    if (ctx->trustedCAs != NULL)
        sk_X509_pop_free(ctx->trustedCAs, X509_free);

    if (ctx->intermediateCAs != NULL)
        sk_X509_pop_free(ctx->intermediateCAs, X509_free);

    if (ctx->revoked != NULL)
        sk_X509_CRL_pop_free(ctx->revoked, X509_CRL_free);
}
