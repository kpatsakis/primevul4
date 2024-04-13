void Curl_nss_cleanup(void)
{
  /* This function isn't required to be threadsafe and this is only done
   * as a safety feature.
   */
  PR_Lock(nss_initlock);
  if(initialized) {
    /* Free references to client certificates held in the SSL session cache.
     * Omitting this hampers destruction of the security module owning
     * the certificates. */
    SSL_ClearSessionCache();

    if(mod && SECSuccess == SECMOD_UnloadUserModule(mod)) {
      SECMOD_DestroyModule(mod);
      mod = NULL;
    }
    NSS_ShutdownContext(nss_context);
    nss_context = NULL;
  }

  /* destroy all CRL items */
  Curl_llist_destroy(nss_crl_list, NULL);
  nss_crl_list = NULL;

  PR_Unlock(nss_initlock);

  PR_DestroyLock(nss_initlock);
  PR_DestroyLock(nss_crllock);
  nss_initlock = NULL;

  initialized = 0;
}
