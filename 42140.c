pkinit_sign_data_fs(krb5_context context,
                    pkinit_identity_crypto_context id_cryptoctx,
                    unsigned char *data,
                    unsigned int data_len,
                    unsigned char **sig,
                    unsigned int *sig_len)
{
    if (create_signature(sig, sig_len, data, data_len,
                         id_cryptoctx->my_key) != 0) {
        pkiDebug("failed to create the signature\n");
        return KRB5KDC_ERR_PREAUTH_FAILED;
    }
    return 0;
}
