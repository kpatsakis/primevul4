openssl_callback_ignore_crls(int ok, X509_STORE_CTX * ctx)
{
    if (!ok) {
        switch (ctx->error) {
        case X509_V_ERR_UNABLE_TO_GET_CRL:
            return 1;
        default:
            return 0;
        }
    }
    return ok;
}
