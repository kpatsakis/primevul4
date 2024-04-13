pkinit_encode_dh_params(BIGNUM *p, BIGNUM *g, BIGNUM *q,
                        unsigned char **buf, unsigned int *buf_len)
{
    krb5_error_code retval = ENOMEM;
    int bufsize = 0, r = 0;
    unsigned char *tmp = NULL;
    ASN1_INTEGER *ap = NULL, *ag = NULL, *aq = NULL;

    if ((ap = BN_to_ASN1_INTEGER(p, NULL)) == NULL)
        goto cleanup;
    if ((ag = BN_to_ASN1_INTEGER(g, NULL)) == NULL)
        goto cleanup;
    if ((aq = BN_to_ASN1_INTEGER(q, NULL)) == NULL)
        goto cleanup;
    bufsize = i2d_ASN1_INTEGER(ap, NULL);
    bufsize += i2d_ASN1_INTEGER(ag, NULL);
    bufsize += i2d_ASN1_INTEGER(aq, NULL);

    r = ASN1_object_size(1, bufsize, V_ASN1_SEQUENCE);

    tmp = *buf = malloc((size_t) r);
    if (tmp == NULL)
        goto cleanup;

    ASN1_put_object(&tmp, 1, bufsize, V_ASN1_SEQUENCE, V_ASN1_UNIVERSAL);

    i2d_ASN1_INTEGER(ap, &tmp);
    i2d_ASN1_INTEGER(ag, &tmp);
    i2d_ASN1_INTEGER(aq, &tmp);

    *buf_len = r;

    retval = 0;

cleanup:
    if (ap != NULL)
        ASN1_INTEGER_free(ap);
    if (ag != NULL)
        ASN1_INTEGER_free(ag);
    if (aq != NULL)
        ASN1_INTEGER_free(aq);

    return retval;
}
