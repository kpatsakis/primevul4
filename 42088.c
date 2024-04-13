crypto_retieve_X509_key_usage(krb5_context context,
                              pkinit_plg_crypto_context plgcctx,
                              pkinit_req_crypto_context reqcctx,
                              X509 *x,
                              unsigned int *ret_ku_bits,
                              unsigned int *ret_eku_bits)
{
    krb5_error_code retval = 0;
    int i;
    unsigned int eku_bits = 0, ku_bits = 0;
    ASN1_BIT_STRING *usage = NULL;

    if (ret_ku_bits == NULL && ret_eku_bits == NULL)
        return EINVAL;

    if (ret_eku_bits)
        *ret_eku_bits = 0;
    else {
        pkiDebug("%s: EKUs not requested, not checking\n", __FUNCTION__);
        goto check_kus;
    }

    /* Start with Extended Key usage */
    i = X509_get_ext_by_NID(x, NID_ext_key_usage, -1);
    if (i >= 0) {
        EXTENDED_KEY_USAGE *eku;

        eku = X509_get_ext_d2i(x, NID_ext_key_usage, NULL, NULL);
        if (eku) {
            for (i = 0; i < sk_ASN1_OBJECT_num(eku); i++) {
                ASN1_OBJECT *certoid;
                certoid = sk_ASN1_OBJECT_value(eku, i);
                if ((OBJ_cmp(certoid, plgcctx->id_pkinit_KPClientAuth)) == 0)
                    eku_bits |= PKINIT_EKU_PKINIT;
                else if ((OBJ_cmp(certoid, OBJ_nid2obj(NID_ms_smartcard_login))) == 0)
                    eku_bits |= PKINIT_EKU_MSSCLOGIN;
                else if ((OBJ_cmp(certoid, OBJ_nid2obj(NID_client_auth))) == 0)
                    eku_bits |= PKINIT_EKU_CLIENTAUTH;
                else if ((OBJ_cmp(certoid, OBJ_nid2obj(NID_email_protect))) == 0)
                    eku_bits |= PKINIT_EKU_EMAILPROTECTION;
            }
            EXTENDED_KEY_USAGE_free(eku);
        }
    }
    pkiDebug("%s: returning eku 0x%08x\n", __FUNCTION__, eku_bits);
    *ret_eku_bits = eku_bits;

check_kus:
    /* Now the Key Usage bits */
    if (ret_ku_bits)
        *ret_ku_bits = 0;
    else {
        pkiDebug("%s: KUs not requested, not checking\n", __FUNCTION__);
        goto out;
    }

    /* Make sure usage exists before checking bits */
    X509_check_ca(x);
    usage = X509_get_ext_d2i(x, NID_key_usage, NULL, NULL);
    if (usage) {
        if (!ku_reject(x, X509v3_KU_DIGITAL_SIGNATURE))
            ku_bits |= PKINIT_KU_DIGITALSIGNATURE;
        if (!ku_reject(x, X509v3_KU_KEY_ENCIPHERMENT))
            ku_bits |= PKINIT_KU_KEYENCIPHERMENT;
        ASN1_BIT_STRING_free(usage);
    }

    pkiDebug("%s: returning ku 0x%08x\n", __FUNCTION__, ku_bits);
    *ret_ku_bits = ku_bits;
    retval = 0;
out:
    return retval;
}
