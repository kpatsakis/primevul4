int Curl_nss_init(void)
{
  /* curl_global_init() is not thread-safe so this test is ok */
  if(nss_initlock == NULL) {
    PR_Init(PR_USER_THREAD, PR_PRIORITY_NORMAL, 256);
    nss_initlock = PR_NewLock();
    nss_crllock = PR_NewLock();
  }

  /* We will actually initialize NSS later */

  return 1;
}
