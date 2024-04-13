crypto_check_cert_eku(krb5_context context,
                      pkinit_plg_crypto_context plgctx,
                      pkinit_req_crypto_context reqctx,
                      pkinit_identity_crypto_context idctx,
                      int checking_kdc_cert,
                      int allow_secondary_usage,
                      int *valid_eku)
{
    char buf[DN_BUF_LEN];
    int found_eku = 0;
    krb5_error_code retval = EINVAL;
    int i;

    *valid_eku = 0;
    if (reqctx->received_cert == NULL)
        goto cleanup;

    X509_NAME_oneline(X509_get_subject_name(reqctx->received_cert),
                      buf, sizeof(buf));
    pkiDebug("%s: looking for EKUs in cert = %s\n", __FUNCTION__, buf);

    if ((i = X509_get_ext_by_NID(reqctx->received_cert,
                                 NID_ext_key_usage, -1)) >= 0) {
        EXTENDED_KEY_USAGE *extusage;

        extusage = X509_get_ext_d2i(reqctx->received_cert, NID_ext_key_usage,
                                    NULL, NULL);
        if (extusage) {
            pkiDebug("%s: found eku info in the cert\n", __FUNCTION__);
            for (i = 0; found_eku == 0 && i < sk_ASN1_OBJECT_num(extusage); i++) {
                ASN1_OBJECT *tmp_oid;

                tmp_oid = sk_ASN1_OBJECT_value(extusage, i);
                pkiDebug("%s: checking eku %d of %d, allow_secondary = %d\n",
                         __FUNCTION__, i+1, sk_ASN1_OBJECT_num(extusage),
                         allow_secondary_usage);
                if (checking_kdc_cert) {
                    if ((OBJ_cmp(tmp_oid, plgctx->id_pkinit_KPKdc) == 0)
                        || (allow_secondary_usage
                            && OBJ_cmp(tmp_oid, plgctx->id_kp_serverAuth) == 0))
                        found_eku = 1;
                } else {
                    if ((OBJ_cmp(tmp_oid, plgctx->id_pkinit_KPClientAuth) == 0)
                        || (allow_secondary_usage
                            && OBJ_cmp(tmp_oid, plgctx->id_ms_kp_sc_logon) == 0))
                        found_eku = 1;
                }
            }
        }
        EXTENDED_KEY_USAGE_free(extusage);

        if (found_eku) {
            ASN1_BIT_STRING *usage = NULL;
            pkiDebug("%s: found acceptable EKU, checking for digitalSignature\n", __FUNCTION__);

            /* check that digitalSignature KeyUsage is present */
            X509_check_ca(reqctx->received_cert);
            if ((usage = X509_get_ext_d2i(reqctx->received_cert,
                                          NID_key_usage, NULL, NULL))) {

                if (!ku_reject(reqctx->received_cert,
                               X509v3_KU_DIGITAL_SIGNATURE)) {
                    pkiDebug("%s: found digitalSignature KU\n",
                             __FUNCTION__);
                    *valid_eku = 1;
                } else
                    pkiDebug("%s: didn't find digitalSignature KU\n",
                             __FUNCTION__);
            }
            ASN1_BIT_STRING_free(usage);
        }
    }
    retval = 0;
cleanup:
    pkiDebug("%s: returning retval %d, valid_eku %d\n",
             __FUNCTION__, retval, *valid_eku);
    return retval;
}
