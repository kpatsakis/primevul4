static CURLcode nss_cache_crl(SECItem *crl_der)
{
  CERTCertDBHandle *db = CERT_GetDefaultCertDB();
  CERTSignedCrl *crl = SEC_FindCrlByDERCert(db, crl_der, 0);
  if(crl) {
    /* CRL already cached */
    SEC_DestroyCrl(crl);
    SECITEM_FreeItem(crl_der, PR_TRUE);
    return CURLE_OK;
  }

  /* acquire lock before call of CERT_CacheCRL() and accessing nss_crl_list */
  PR_Lock(nss_crllock);

  /* store the CRL item so that we can free it in Curl_nss_cleanup() */
  if(!Curl_llist_insert_next(nss_crl_list, nss_crl_list->tail, crl_der)) {
    SECITEM_FreeItem(crl_der, PR_TRUE);
    PR_Unlock(nss_crllock);
    return CURLE_OUT_OF_MEMORY;
  }

  if(SECSuccess != CERT_CacheCRL(db, crl_der)) {
    /* unable to cache CRL */
    PR_Unlock(nss_crllock);
    return CURLE_SSL_CRL_BADFILE;
  }

  /* we need to clear session cache, so that the CRL could take effect */
  SSL_ClearSessionCache();
  PR_Unlock(nss_crllock);
  return CURLE_OK;
}
