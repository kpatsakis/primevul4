int X509_get_pubkey_parameters(EVP_PKEY *pkey, STACK_OF(X509) *chain)
{
    EVP_PKEY *ktmp = NULL, *ktmp2;
    int i, j;

    if ((pkey != NULL) && !EVP_PKEY_missing_parameters(pkey))
        return 1;

    for (i = 0; i < sk_X509_num(chain); i++) {
        ktmp = X509_get_pubkey(sk_X509_value(chain, i));
        if (ktmp == NULL) {
            X509err(X509_F_X509_GET_PUBKEY_PARAMETERS,
                    X509_R_UNABLE_TO_GET_CERTS_PUBLIC_KEY);
            return 0;
        }
        if (!EVP_PKEY_missing_parameters(ktmp))
            break;
        EVP_PKEY_free(ktmp);
        ktmp = NULL;
    }
    if (ktmp == NULL) {
        X509err(X509_F_X509_GET_PUBKEY_PARAMETERS,
                X509_R_UNABLE_TO_FIND_PARAMETERS_IN_CHAIN);
        return 0;
    }

    /* first, populate the other certs */
    for (j = i - 1; j >= 0; j--) {
        ktmp2 = X509_get_pubkey(sk_X509_value(chain, j));
        EVP_PKEY_copy_parameters(ktmp2, ktmp);
        EVP_PKEY_free(ktmp2);
    }

    if (pkey != NULL)
        EVP_PKEY_copy_parameters(pkey, ktmp);
    EVP_PKEY_free(ktmp);
    return 1;
}
