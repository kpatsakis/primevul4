pkinit_find_private_key(pkinit_identity_crypto_context id_cryptoctx,
                        CK_ATTRIBUTE_TYPE usage,
                        CK_OBJECT_HANDLE *objp)
{
    CK_OBJECT_CLASS cls;
    CK_ATTRIBUTE attrs[4];
    CK_ULONG count;
    CK_KEY_TYPE keytype;
    unsigned int nattrs = 0;
    int r;
#ifdef PKINIT_USE_KEY_USAGE
    CK_BBOOL true_false;
#endif

    cls = CKO_PRIVATE_KEY;
    attrs[nattrs].type = CKA_CLASS;
    attrs[nattrs].pValue = &cls;
    attrs[nattrs].ulValueLen = sizeof cls;
    nattrs++;

#ifdef PKINIT_USE_KEY_USAGE
    /*
     * Some cards get confused if you try to specify a key usage,
     * so don't, and hope for the best. This will fail if you have
     * several keys with the same id and different usages but I have
     * not seen this on real cards.
     */
    true_false = TRUE;
    attrs[nattrs].type = usage;
    attrs[nattrs].pValue = &true_false;
    attrs[nattrs].ulValueLen = sizeof true_false;
    nattrs++;
#endif

    keytype = CKK_RSA;
    attrs[nattrs].type = CKA_KEY_TYPE;
    attrs[nattrs].pValue = &keytype;
    attrs[nattrs].ulValueLen = sizeof keytype;
    nattrs++;

    attrs[nattrs].type = CKA_ID;
    attrs[nattrs].pValue = id_cryptoctx->cert_id;
    attrs[nattrs].ulValueLen = id_cryptoctx->cert_id_len;
    nattrs++;

    r = id_cryptoctx->p11->C_FindObjectsInit(id_cryptoctx->session, attrs, nattrs);
    if (r != CKR_OK) {
        pkiDebug("krb5_pkinit_sign_data: C_FindObjectsInit: %s\n",
                 pkinit_pkcs11_code_to_text(r));
        return KRB5KDC_ERR_PREAUTH_FAILED;
    }

    r = id_cryptoctx->p11->C_FindObjects(id_cryptoctx->session, objp, 1, &count);
    id_cryptoctx->p11->C_FindObjectsFinal(id_cryptoctx->session);
    pkiDebug("found %d private keys (%s)\n", (int) count, pkinit_pkcs11_code_to_text(r));
    if (r != CKR_OK || count < 1)
        return KRB5KDC_ERR_PREAUTH_FAILED;
    return 0;
}
