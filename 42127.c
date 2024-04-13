pkinit_identity_set_prompter(pkinit_identity_crypto_context id_cryptoctx,
                             krb5_prompter_fct prompter,
                             void *prompter_data)
{
    id_cryptoctx->prompter = prompter;
    id_cryptoctx->prompter_data = prompter_data;

    return 0;
}
