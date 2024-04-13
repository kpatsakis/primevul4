static ASN1_TYPE *get_attribute(STACK_OF(X509_ATTRIBUTE) *sk, int nid)
{
    int idx;
    X509_ATTRIBUTE *xa;
    idx = X509at_get_attr_by_NID(sk, nid, -1);
    xa = X509at_get_attr(sk, idx);
    return X509_ATTRIBUTE_get0_type(xa, 0);
}
