pkinit_decode_dh_params(DH ** a, unsigned char **pp, unsigned int len)
{
    ASN1_INTEGER ai, *aip = NULL;
    long length = (long) len;

    M_ASN1_D2I_vars(a, DH *, DH_new);

    M_ASN1_D2I_Init();
    M_ASN1_D2I_start_sequence();
    aip = &ai;
    ai.data = NULL;
    ai.length = 0;
    M_ASN1_D2I_get_x(ASN1_INTEGER, aip, d2i_ASN1_INTEGER);
    if (aip == NULL)
        return NULL;
    else {
        (*a)->p = ASN1_INTEGER_to_BN(aip, NULL);
        if ((*a)->p == NULL)
            return NULL;
        if (ai.data != NULL) {
            OPENSSL_free(ai.data);
            ai.data = NULL;
            ai.length = 0;
        }
    }
    M_ASN1_D2I_get_x(ASN1_INTEGER, aip, d2i_ASN1_INTEGER);
    if (aip == NULL)
        return NULL;
    else {
        (*a)->g = ASN1_INTEGER_to_BN(aip, NULL);
        if ((*a)->g == NULL)
            return NULL;
        if (ai.data != NULL) {
            OPENSSL_free(ai.data);
            ai.data = NULL;
            ai.length = 0;
        }

    }
    M_ASN1_D2I_get_x(ASN1_INTEGER, aip, d2i_ASN1_INTEGER);
    if (aip == NULL)
        return NULL;
    else {
        (*a)->q = ASN1_INTEGER_to_BN(aip, NULL);
        if ((*a)->q == NULL)
            return NULL;
        if (ai.data != NULL) {
            OPENSSL_free(ai.data);
            ai.data = NULL;
            ai.length = 0;
        }

    }
    M_ASN1_D2I_end_sequence();
    M_ASN1_D2I_Finish(a, DH_free, 0);

}
