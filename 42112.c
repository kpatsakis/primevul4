pkinit_decode_data_fs(krb5_context context,
                      pkinit_identity_crypto_context id_cryptoctx,
                      unsigned char *data,
                      unsigned int data_len,
                      unsigned char **decoded_data,
                      unsigned int *decoded_data_len)
{
    if (decode_data(decoded_data, decoded_data_len, data, data_len,
                    id_cryptoctx->my_key, sk_X509_value(id_cryptoctx->my_certs,
                                                        id_cryptoctx->cert_index)) <= 0) {
        pkiDebug("failed to decode data\n");
        return KRB5KDC_ERR_PREAUTH_FAILED;
    }
    return 0;
}
