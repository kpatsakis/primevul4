crypto_cert_release(krb5_context context,
                    pkinit_cert_handle ch)
{
    struct _pkinit_cert_data *cd = (struct _pkinit_cert_data *)ch;
    if (cd == NULL || cd->magic != CERT_MAGIC)
        return EINVAL;
    free(cd);
    return 0;
}
