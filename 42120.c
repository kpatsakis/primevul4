pkinit_fini_pkcs11(pkinit_identity_crypto_context ctx)
{
#ifndef WITHOUT_PKCS11
    if (ctx == NULL)
        return;

    if (ctx->p11 != NULL) {
        if (ctx->session) {
            ctx->p11->C_CloseSession(ctx->session);
            ctx->session = CK_INVALID_HANDLE;
        }
        ctx->p11->C_Finalize(NULL_PTR);
        ctx->p11 = NULL;
    }
    if (ctx->p11_module != NULL) {
        pkinit_C_UnloadModule(ctx->p11_module);
        ctx->p11_module = NULL;
    }
    free(ctx->p11_module_name);
    free(ctx->token_label);
    free(ctx->cert_id);
    free(ctx->cert_label);
#endif
}
