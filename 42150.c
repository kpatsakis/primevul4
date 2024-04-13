wrap_signeddata(unsigned char *data, unsigned int data_len,
                unsigned char **out, unsigned int *out_len)
{

    unsigned int orig_len = 0, oid_len = 0, tot_len = 0;
    ASN1_OBJECT *oid = NULL;
    unsigned char *p = NULL;

    /* Get length to wrap the original data with SEQUENCE tag */
    tot_len = orig_len = ASN1_object_size(1, (int)data_len, V_ASN1_SEQUENCE);

    /* Add the signedData OID and adjust lengths */
    oid = OBJ_nid2obj(NID_pkcs7_signed);
    oid_len = i2d_ASN1_OBJECT(oid, NULL);

    tot_len = ASN1_object_size(1, (int)(orig_len+oid_len), V_ASN1_SEQUENCE);

    p = *out = malloc(tot_len);
    if (p == NULL) return -1;

    ASN1_put_object(&p, 1, (int)(orig_len+oid_len),
                    V_ASN1_SEQUENCE, V_ASN1_UNIVERSAL);

    i2d_ASN1_OBJECT(oid, &p);

    ASN1_put_object(&p, 1, (int)data_len, 0, V_ASN1_CONTEXT_SPECIFIC);
    memcpy(p, data, data_len);

    *out_len = tot_len;

    return 0;
}
