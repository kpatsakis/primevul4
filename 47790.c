static int pkcs7_cmp_ri(PKCS7_RECIP_INFO *ri, X509 *pcert)
{
    int ret;
    ret = X509_NAME_cmp(ri->issuer_and_serial->issuer,
                        pcert->cert_info->issuer);
    if (ret)
        return ret;
    return ASN1_INTEGER_cmp(pcert->cert_info->serialNumber,
                              ri->issuer_and_serial->serial);
}
