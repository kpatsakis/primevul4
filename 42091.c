crypto_retrieve_signer_identity(krb5_context context,
                                pkinit_identity_crypto_context id_cryptoctx,
                                const char **identity)
{
    *identity = id_cryptoctx->identity;
    if (*identity == NULL)
        return ENOENT;
    return 0;
}
