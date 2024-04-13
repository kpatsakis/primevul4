int PKCS7_set_attributes(PKCS7_SIGNER_INFO *p7si,
                         STACK_OF(X509_ATTRIBUTE) *sk)
{
    int i;

    sk_X509_ATTRIBUTE_pop_free(p7si->unauth_attr, X509_ATTRIBUTE_free);
    p7si->unauth_attr = sk_X509_ATTRIBUTE_dup(sk);
    if (p7si->unauth_attr == NULL)
        return 0;
    for (i = 0; i < sk_X509_ATTRIBUTE_num(sk); i++) {
        if ((sk_X509_ATTRIBUTE_set(p7si->unauth_attr, i,
                                   X509_ATTRIBUTE_dup(sk_X509_ATTRIBUTE_value
                                                      (sk, i))))
            == NULL)
            return (0);
    }
    return (1);
}
