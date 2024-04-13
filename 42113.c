pkinit_decode_data_pkcs11(krb5_context context,
                          pkinit_identity_crypto_context id_cryptoctx,
                          unsigned char *data,
                          unsigned int data_len,
                          unsigned char **decoded_data,
                          unsigned int *decoded_data_len)
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

    pkinit_find_private_key(id_cryptoctx, CKA_DECRYPT, &obj);

    mech.mechanism = CKM_RSA_PKCS;
    mech.pParameter = NULL;
    mech.ulParameterLen = 0;

    if ((r = id_cryptoctx->p11->C_DecryptInit(id_cryptoctx->session, &mech,
                                              obj)) != CKR_OK) {
        pkiDebug("C_DecryptInit: 0x%x\n", (int) r);
        return KRB5KDC_ERR_PREAUTH_FAILED;
    }
    pkiDebug("data_len = %d\n", data_len);
    cp = malloc((size_t) data_len);
    if (cp == NULL)
        return ENOMEM;
    len = data_len;
    pkiDebug("session %p edata %p edata_len %d data %p datalen @%p %d\n",
             (void *) id_cryptoctx->session, (void *) data, (int) data_len,
             (void *) cp, (void *) &len, (int) len);
    if ((r = pkinit_C_Decrypt(id_cryptoctx, data, (CK_ULONG) data_len,
                              cp, &len)) != CKR_OK) {
        pkiDebug("C_Decrypt: %s\n", pkinit_pkcs11_code_to_text(r));
        if (r == CKR_BUFFER_TOO_SMALL)
            pkiDebug("decrypt %d needs %d\n", (int) data_len, (int) len);
        return KRB5KDC_ERR_PREAUTH_FAILED;
    }
    pkiDebug("decrypt %d -> %d\n", (int) data_len, (int) len);
    *decoded_data_len = len;
    *decoded_data = cp;

    return 0;
}
