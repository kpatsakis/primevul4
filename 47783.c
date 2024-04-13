PKCS7_ISSUER_AND_SERIAL *PKCS7_get_issuer_and_serial(PKCS7 *p7, int idx)
{
    STACK_OF(PKCS7_RECIP_INFO) *rsk;
    PKCS7_RECIP_INFO *ri;
    int i;

    i = OBJ_obj2nid(p7->type);
    if (i != NID_pkcs7_signedAndEnveloped)
        return NULL;
    if (p7->d.signed_and_enveloped == NULL)
        return NULL;
    rsk = p7->d.signed_and_enveloped->recipientinfo;
    if (rsk == NULL)
        return NULL;
    ri = sk_PKCS7_RECIP_INFO_value(rsk, 0);
    if (sk_PKCS7_RECIP_INFO_num(rsk) <= idx)
        return (NULL);
    ri = sk_PKCS7_RECIP_INFO_value(rsk, idx);
    return (ri->issuer_and_serial);
}
