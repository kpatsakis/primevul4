crypto_cert_iteration_end(krb5_context context,
                          pkinit_cert_iter_handle ih)
{
    struct _pkinit_cert_iter_data *id = (struct _pkinit_cert_iter_data *)ih;

    if (id == NULL || id->magic != ITER_MAGIC)
        return EINVAL;
    free(ih);
    return 0;
}
