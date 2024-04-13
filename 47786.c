int PKCS7_signatureVerify(BIO *bio, PKCS7 *p7, PKCS7_SIGNER_INFO *si,
                          X509 *x509)
{
    ASN1_OCTET_STRING *os;
    EVP_MD_CTX mdc_tmp, *mdc;
    int ret = 0, i;
    int md_type;
    STACK_OF(X509_ATTRIBUTE) *sk;
    BIO *btmp;
    EVP_PKEY *pkey;

    EVP_MD_CTX_init(&mdc_tmp);

    if (!PKCS7_type_is_signed(p7) && !PKCS7_type_is_signedAndEnveloped(p7)) {
        PKCS7err(PKCS7_F_PKCS7_SIGNATUREVERIFY, PKCS7_R_WRONG_PKCS7_TYPE);
        goto err;
    }

    md_type = OBJ_obj2nid(si->digest_alg->algorithm);

    btmp = bio;
    for (;;) {
        if ((btmp == NULL) ||
            ((btmp = BIO_find_type(btmp, BIO_TYPE_MD)) == NULL)) {
            PKCS7err(PKCS7_F_PKCS7_SIGNATUREVERIFY,
                     PKCS7_R_UNABLE_TO_FIND_MESSAGE_DIGEST);
            goto err;
        }
        BIO_get_md_ctx(btmp, &mdc);
        if (mdc == NULL) {
            PKCS7err(PKCS7_F_PKCS7_SIGNATUREVERIFY, ERR_R_INTERNAL_ERROR);
            goto err;
        }
        if (EVP_MD_CTX_type(mdc) == md_type)
            break;
        /*
         * Workaround for some broken clients that put the signature OID
         * instead of the digest OID in digest_alg->algorithm
         */
        if (EVP_MD_pkey_type(EVP_MD_CTX_md(mdc)) == md_type)
            break;
        btmp = BIO_next(btmp);
    }

    /*
     * mdc is the digest ctx that we want, unless there are attributes, in
     * which case the digest is the signed attributes
     */
    if (!EVP_MD_CTX_copy_ex(&mdc_tmp, mdc))
        goto err;

    sk = si->auth_attr;
    if ((sk != NULL) && (sk_X509_ATTRIBUTE_num(sk) != 0)) {
        unsigned char md_dat[EVP_MAX_MD_SIZE], *abuf = NULL;
        unsigned int md_len;
        int alen;
        ASN1_OCTET_STRING *message_digest;

        if (!EVP_DigestFinal_ex(&mdc_tmp, md_dat, &md_len))
            goto err;
        message_digest = PKCS7_digest_from_attributes(sk);
        if (!message_digest) {
            PKCS7err(PKCS7_F_PKCS7_SIGNATUREVERIFY,
                     PKCS7_R_UNABLE_TO_FIND_MESSAGE_DIGEST);
            goto err;
        }
        if ((message_digest->length != (int)md_len) ||
            (memcmp(message_digest->data, md_dat, md_len))) {
            PKCS7err(PKCS7_F_PKCS7_SIGNATUREVERIFY, PKCS7_R_DIGEST_FAILURE);
            ret = -1;
            goto err;
        }

        if (!EVP_VerifyInit_ex(&mdc_tmp, EVP_get_digestbynid(md_type), NULL))
            goto err;

        alen = ASN1_item_i2d((ASN1_VALUE *)sk, &abuf,
                             ASN1_ITEM_rptr(PKCS7_ATTR_VERIFY));
        if (alen <= 0) {
            PKCS7err(PKCS7_F_PKCS7_SIGNATUREVERIFY, ERR_R_ASN1_LIB);
            ret = -1;
            goto err;
        }
        if (!EVP_VerifyUpdate(&mdc_tmp, abuf, alen))
            goto err;

        OPENSSL_free(abuf);
    }

    os = si->enc_digest;
    pkey = X509_get_pubkey(x509);
    if (!pkey) {
        ret = -1;
        goto err;
    }

    i = EVP_VerifyFinal(&mdc_tmp, os->data, os->length, pkey);
    EVP_PKEY_free(pkey);
    if (i <= 0) {
        PKCS7err(PKCS7_F_PKCS7_SIGNATUREVERIFY, PKCS7_R_SIGNATURE_FAILURE);
        ret = -1;
        goto err;
    }
    ret = 1;
 err:
    EVP_MD_CTX_cleanup(&mdc_tmp);
    return (ret);
}
