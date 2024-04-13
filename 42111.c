pkinit_decode_data(krb5_context context,
                   pkinit_identity_crypto_context id_cryptoctx,
                   unsigned char *data,
                   unsigned int data_len,
                   unsigned char **decoded_data,
                   unsigned int *decoded_data_len)
{
    krb5_error_code retval = KRB5KDC_ERR_PREAUTH_FAILED;

    if (id_cryptoctx->pkcs11_method != 1)
        retval = pkinit_decode_data_fs(context, id_cryptoctx, data, data_len,
                                       decoded_data, decoded_data_len);
#ifndef WITHOUT_PKCS11
    else
        retval = pkinit_decode_data_pkcs11(context, id_cryptoctx, data,
                                           data_len, decoded_data, decoded_data_len);
#endif

    return retval;
}
