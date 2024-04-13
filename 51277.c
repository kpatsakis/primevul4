static CURLcode nss_init(struct Curl_easy *data)
{
  char *cert_dir;
  struct_stat st;
  CURLcode result;

  if(initialized)
    return CURLE_OK;

  /* list of all CRL items we need to destroy in Curl_nss_cleanup() */
  nss_crl_list = Curl_llist_alloc(nss_destroy_crl_item);
  if(!nss_crl_list)
    return CURLE_OUT_OF_MEMORY;

  /* First we check if $SSL_DIR points to a valid dir */
  cert_dir = getenv("SSL_DIR");
  if(cert_dir) {
    if((stat(cert_dir, &st) != 0) ||
        (!S_ISDIR(st.st_mode))) {
      cert_dir = NULL;
    }
  }

  /* Now we check if the default location is a valid dir */
  if(!cert_dir) {
    if((stat(SSL_DIR, &st) == 0) &&
        (S_ISDIR(st.st_mode))) {
      cert_dir = (char *)SSL_DIR;
    }
  }

  if(nspr_io_identity == PR_INVALID_IO_LAYER) {
    /* allocate an identity for our own NSPR I/O layer */
    nspr_io_identity = PR_GetUniqueIdentity("libcurl");
    if(nspr_io_identity == PR_INVALID_IO_LAYER)
      return CURLE_OUT_OF_MEMORY;

    /* the default methods just call down to the lower I/O layer */
    memcpy(&nspr_io_methods, PR_GetDefaultIOMethods(), sizeof nspr_io_methods);

    /* override certain methods in the table by our wrappers */
    nspr_io_methods.recv  = nspr_io_recv;
    nspr_io_methods.send  = nspr_io_send;
    nspr_io_methods.close = nspr_io_close;
  }

  result = nss_init_core(data, cert_dir);
  if(result)
    return result;

  if(!any_cipher_enabled())
    NSS_SetDomesticPolicy();

  initialized = 1;

  return CURLE_OK;
}
