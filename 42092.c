decode_data(unsigned char **out_data, unsigned int *out_data_len,
            unsigned char *data, unsigned int data_len,
            EVP_PKEY *pkey, X509 *cert)
{
    krb5_error_code retval = ENOMEM;
    unsigned char *buf = NULL;
    int buf_len = 0;

    if (cert && !X509_check_private_key(cert, pkey)) {
        pkiDebug("private key does not match certificate\n");
        goto cleanup;
    }

    buf_len = EVP_PKEY_size(pkey);
    buf = malloc((size_t) buf_len + 10);
    if (buf == NULL)
        goto cleanup;

#if OPENSSL_VERSION_NUMBER >= 0x00909000L
    retval = EVP_PKEY_decrypt_old(buf, data, (int)data_len, pkey);
#else
    retval = EVP_PKEY_decrypt(buf, data, (int)data_len, pkey);
#endif
    if (retval <= 0) {
        pkiDebug("unable to decrypt received data (len=%d)\n", data_len);
        goto cleanup;
    }
    *out_data = buf;
    *out_data_len = retval;

cleanup:
    if (retval == ENOMEM)
        free(buf);

    return retval;
}
