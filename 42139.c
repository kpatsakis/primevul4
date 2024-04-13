pkinit_sign_data(krb5_context context,
                 pkinit_identity_crypto_context id_cryptoctx,
                 unsigned char *data,
                 unsigned int data_len,
                 unsigned char **sig,
                 unsigned int *sig_len)
{
    krb5_error_code retval = KRB5KDC_ERR_PREAUTH_FAILED;

    if (id_cryptoctx == NULL || id_cryptoctx->pkcs11_method != 1)
        retval = pkinit_sign_data_fs(context, id_cryptoctx, data, data_len,
                                     sig, sig_len);
#ifndef WITHOUT_PKCS11
    else
        retval = pkinit_sign_data_pkcs11(context, id_cryptoctx, data, data_len,
                                         sig, sig_len);
#endif

    return retval;
}
