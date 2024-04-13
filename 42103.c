pkinit_C_Decrypt(pkinit_identity_crypto_context id_cryptoctx,
                 CK_BYTE_PTR pEncryptedData,
                 CK_ULONG  ulEncryptedDataLen,
                 CK_BYTE_PTR pData,
                 CK_ULONG_PTR pulDataLen)
{
    CK_RV rv = CKR_OK;

    rv = id_cryptoctx->p11->C_Decrypt(id_cryptoctx->session, pEncryptedData,
                                      ulEncryptedDataLen, pData, pulDataLen);
    if (rv == CKR_OK) {
        pkiDebug("pData %p *pulDataLen %d\n", (void *) pData,
                 (int) *pulDataLen);
    }
    return rv;
}
