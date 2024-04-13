krb5_build_principal_va(krb5_context context,
                        krb5_principal princ,
                        unsigned int rlen,
                        const char *realm,
                        va_list ap)
{
    return build_principal_va(context, princ, rlen, realm, ap);
}
