get_key(char *filename, EVP_PKEY **retkey)
{
    EVP_PKEY *pkey = NULL;
    BIO *tmp = NULL;
    int code;
    krb5_error_code retval;

    if (filename == NULL || retkey == NULL)
        return EINVAL;

    tmp = BIO_new(BIO_s_file());
    if (tmp == NULL)
        return ENOMEM;

    code = BIO_read_filename(tmp, filename);
    if (code == 0) {
        retval = errno;
        goto cleanup;
    }
    pkey = (EVP_PKEY *) PEM_read_bio_PrivateKey(tmp, NULL, NULL, NULL);
    if (pkey == NULL) {
        retval = EIO;
        pkiDebug("failed to read private key from %s\n", filename);
        goto cleanup;
    }
    *retkey = pkey;
    retval = 0;
cleanup:
    if (tmp != NULL)
        BIO_free(tmp);
    return retval;
}
