int TS_ext_print_bio(BIO *bio, const STACK_OF(X509_EXTENSION) *extensions)
{
    int i, critical, n;
    X509_EXTENSION *ex;
    ASN1_OBJECT *obj;

    BIO_printf(bio, "Extensions:\n");
    n = X509v3_get_ext_count(extensions);
    for (i = 0; i < n; i++) {
        ex = X509v3_get_ext(extensions, i);
        obj = X509_EXTENSION_get_object(ex);
        i2a_ASN1_OBJECT(bio, obj);
        critical = X509_EXTENSION_get_critical(ex);
        BIO_printf(bio, ": %s\n", critical ? "critical" : "");
        if (!X509V3_EXT_print(bio, ex, 0, 4)) {
            BIO_printf(bio, "%4s", "");
            ASN1_STRING_print(bio, X509_EXTENSION_get_data(ex));
        }
        BIO_write(bio, "\n", 1);
    }

    return 1;
}
