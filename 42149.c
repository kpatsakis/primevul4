wrap_signeddata(unsigned char *data, unsigned int data_len,
                unsigned char **out, unsigned int *out_len,
                int is_longhorn_server)
{

    unsigned int oid_len = 0, tot_len = 0, wrap_len = 0, tag_len = 0;
    ASN1_OBJECT *oid = NULL;
    unsigned char *p = NULL;

    pkiDebug("%s: This is the Longhorn version and is_longhorn_server = %d\n",
             __FUNCTION__, is_longhorn_server);

    /* New longhorn is missing another sequence */
    if (is_longhorn_server == 1)
        wrap_len = ASN1_object_size(1, (int)(data_len), V_ASN1_SEQUENCE);
    else
        wrap_len = data_len;

    /* Get length to wrap the original data with SEQUENCE tag */
    tag_len = ASN1_object_size(1, (int)wrap_len, V_ASN1_SEQUENCE);

    /* Always add oid */
    oid = OBJ_nid2obj(NID_pkcs7_signed);
    oid_len = i2d_ASN1_OBJECT(oid, NULL);
    oid_len += tag_len;

    tot_len = ASN1_object_size(1, (int)(oid_len), V_ASN1_SEQUENCE);

    p = *out = malloc(tot_len);
    if (p == NULL)
        return -1;

    ASN1_put_object(&p, 1, (int)(oid_len),
                    V_ASN1_SEQUENCE, V_ASN1_UNIVERSAL);

    i2d_ASN1_OBJECT(oid, &p);

    ASN1_put_object(&p, 1, (int)wrap_len, 0, V_ASN1_CONTEXT_SPECIFIC);

    /* Wrap in extra seq tag */
    if (is_longhorn_server == 1) {
        ASN1_put_object(&p, 1, (int)data_len, V_ASN1_SEQUENCE, V_ASN1_UNIVERSAL);
    }
    memcpy(p, data, data_len);

    *out_len = tot_len;

    return 0;
}
