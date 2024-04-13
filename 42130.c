pkinit_init_pkcs11(pkinit_identity_crypto_context ctx)
{
    krb5_error_code retval = ENOMEM;

#ifndef WITHOUT_PKCS11
    ctx->p11_module_name = strdup(PKCS11_MODNAME);
    if (ctx->p11_module_name == NULL)
        return retval;
    ctx->p11_module = NULL;
    ctx->slotid = PK_NOSLOT;
    ctx->token_label = NULL;
    ctx->cert_label = NULL;
    ctx->session = CK_INVALID_HANDLE;
    ctx->p11 = NULL;
#endif
    ctx->pkcs11_method = 0;

    retval = 0;
    return retval;
}
