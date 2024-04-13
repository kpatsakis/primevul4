create_signature(unsigned char **sig, unsigned int *sig_len,
                 unsigned char *data, unsigned int data_len, EVP_PKEY *pkey)
{
    krb5_error_code retval = ENOMEM;
    EVP_MD_CTX md_ctx;

    if (pkey == NULL)
        return retval;

    EVP_VerifyInit(&md_ctx, EVP_sha1());
    EVP_SignUpdate(&md_ctx, data, data_len);
    *sig_len = EVP_PKEY_size(pkey);
    if ((*sig = malloc(*sig_len)) == NULL)
        goto cleanup;
    EVP_SignFinal(&md_ctx, *sig, sig_len, pkey);

    retval = 0;

cleanup:
    EVP_MD_CTX_cleanup(&md_ctx);

    return retval;
}
