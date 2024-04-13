openssl_callback(int ok, X509_STORE_CTX * ctx)
{
#ifdef DEBUG
    if (!ok) {
        char buf[DN_BUF_LEN];

        X509_NAME_oneline(X509_get_subject_name(ctx->current_cert), buf, sizeof(buf));
        pkiDebug("cert = %s\n", buf);
        pkiDebug("callback function: %d (%s)\n", ctx->error,
                 X509_verify_cert_error_string(ctx->error));
    }
#endif
    return ok;
}
