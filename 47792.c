static int pkcs7_encode_rinfo(PKCS7_RECIP_INFO *ri,
                              unsigned char *key, int keylen)
{
    EVP_PKEY_CTX *pctx = NULL;
    EVP_PKEY *pkey = NULL;
    unsigned char *ek = NULL;
    int ret = 0;
    size_t eklen;

    pkey = X509_get_pubkey(ri->cert);

    if (!pkey)
        return 0;

    pctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!pctx)
        return 0;

    if (EVP_PKEY_encrypt_init(pctx) <= 0)
        goto err;

    if (EVP_PKEY_CTX_ctrl(pctx, -1, EVP_PKEY_OP_ENCRYPT,
                          EVP_PKEY_CTRL_PKCS7_ENCRYPT, 0, ri) <= 0) {
        PKCS7err(PKCS7_F_PKCS7_ENCODE_RINFO, PKCS7_R_CTRL_ERROR);
        goto err;
    }

    if (EVP_PKEY_encrypt(pctx, NULL, &eklen, key, keylen) <= 0)
        goto err;

    ek = OPENSSL_malloc(eklen);

    if (ek == NULL) {
        PKCS7err(PKCS7_F_PKCS7_ENCODE_RINFO, ERR_R_MALLOC_FAILURE);
        goto err;
    }

    if (EVP_PKEY_encrypt(pctx, ek, &eklen, key, keylen) <= 0)
        goto err;

    ASN1_STRING_set0(ri->enc_key, ek, eklen);
    ek = NULL;

    ret = 1;

 err:
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(pctx);
    OPENSSL_free(ek);
    return ret;

}
