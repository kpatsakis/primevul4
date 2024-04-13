pkinit_sign_data_pkcs11(krb5_context context,
                        pkinit_identity_crypto_context id_cryptoctx,
                        unsigned char *data,
                        unsigned int data_len,
                        unsigned char **sig,
                        unsigned int *sig_len)
{
    CK_OBJECT_HANDLE obj;
    CK_ULONG len;
    CK_MECHANISM mech;
    unsigned char *cp;
    int r;

    if (pkinit_open_session(context, id_cryptoctx)) {
        pkiDebug("can't open pkcs11 session\n");
        return KRB5KDC_ERR_PREAUTH_FAILED;
    }

    pkinit_find_private_key(id_cryptoctx, CKA_SIGN, &obj);

    mech.mechanism = id_cryptoctx->mech;
    mech.pParameter = NULL;
    mech.ulParameterLen = 0;

    if ((r = id_cryptoctx->p11->C_SignInit(id_cryptoctx->session, &mech,
                                           obj)) != CKR_OK) {
        pkiDebug("C_SignInit: %s\n", pkinit_pkcs11_code_to_text(r));
        return KRB5KDC_ERR_PREAUTH_FAILED;
    }

    /*
     * Key len would give an upper bound on sig size, but there's no way to
     * get that. So guess, and if it's too small, re-malloc.
     */
    len = PK_SIGLEN_GUESS;
    cp = malloc((size_t) len);
    if (cp == NULL)
        return ENOMEM;

    r = id_cryptoctx->p11->C_Sign(id_cryptoctx->session, data,
                                  (CK_ULONG) data_len, cp, &len);
    if (r == CKR_BUFFER_TOO_SMALL || (r == CKR_OK && len >= PK_SIGLEN_GUESS)) {
        free(cp);
        pkiDebug("C_Sign realloc %d\n", (int) len);
        cp = malloc((size_t) len);
        r = id_cryptoctx->p11->C_Sign(id_cryptoctx->session, data,
                                      (CK_ULONG) data_len, cp, &len);
    }
    if (r != CKR_OK) {
        pkiDebug("C_Sign: %s\n", pkinit_pkcs11_code_to_text(r));
        return KRB5KDC_ERR_PREAUTH_FAILED;
    }
    pkiDebug("sign %d -> %d\n", (int) data_len, (int) len);
    *sig_len = len;
    *sig = cp;

    return 0;
}
