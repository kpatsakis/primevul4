free_cred_info(krb5_context context,
               pkinit_identity_crypto_context id_cryptoctx,
               struct _pkinit_cred_info *cred)
{
    if (cred != NULL) {
        if (cred->cert != NULL)
            X509_free(cred->cert);
        if (cred->key != NULL)
            EVP_PKEY_free(cred->key);
#ifndef WITHOUT_PKCS11
        free(cred->cert_id);
#endif
        free(cred->name);
        free(cred);
    }
}
