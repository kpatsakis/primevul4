ASN1_OCTET_STRING *PKCS7_digest_from_attributes(STACK_OF(X509_ATTRIBUTE) *sk)
{
    ASN1_TYPE *astype;
    if ((astype = get_attribute(sk, NID_pkcs9_messageDigest)) == NULL)
        return NULL;
    return astype->value.octet_string;
}
