get_cert(char *filename, X509 **retcert)
{
    X509 *cert = NULL;
    BIO *tmp = NULL;
    int code;
    krb5_error_code retval;

    if (filename == NULL || retcert == NULL)
        return EINVAL;

    *retcert = NULL;

    tmp = BIO_new(BIO_s_file());
    if (tmp == NULL)
        return ENOMEM;

    code = BIO_read_filename(tmp, filename);
    if (code == 0) {
        retval = errno;
        goto cleanup;
    }

    cert = (X509 *) PEM_read_bio_X509(tmp, NULL, NULL, NULL);
    if (cert == NULL) {
        retval = EIO;
        pkiDebug("failed to read certificate from %s\n", filename);
        goto cleanup;
    }
    *retcert = cert;
    retval = 0;
cleanup:
    if (tmp != NULL)
        BIO_free(tmp);
    return retval;
}
