krb5_build_principal_alloc_va(krb5_context context,
                              krb5_principal *princ,
                              unsigned int rlen,
                              const char *realm,
                              va_list ap)
{
    krb5_error_code retval = 0;
    krb5_principal p;

    p = malloc(sizeof(krb5_principal_data));
    if (p == NULL)
        return ENOMEM;

    retval = build_principal_va(context, p, rlen, realm, ap);
    if (retval) {
        free(p);
        return retval;
    }

    *princ = p;
    return 0;
}
