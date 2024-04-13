static CURLcode nss_init_core(struct Curl_easy *data, const char *cert_dir)
{
  NSSInitParameters initparams;

  if(nss_context != NULL)
    return CURLE_OK;

  memset((void *) &initparams, '\0', sizeof(initparams));
  initparams.length = sizeof(initparams);

  if(cert_dir) {
    char *certpath = aprintf("sql:%s", cert_dir);
    if(!certpath)
      return CURLE_OUT_OF_MEMORY;

    infof(data, "Initializing NSS with certpath: %s\n", certpath);
    nss_context = NSS_InitContext(certpath, "", "", "", &initparams,
            NSS_INIT_READONLY | NSS_INIT_PK11RELOAD);
    free(certpath);

    if(nss_context != NULL)
      return CURLE_OK;

    infof(data, "Unable to initialize NSS database\n");
  }

  infof(data, "Initializing NSS with certpath: none\n");
  nss_context = NSS_InitContext("", "", "", "", &initparams, NSS_INIT_READONLY
         | NSS_INIT_NOCERTDB   | NSS_INIT_NOMODDB       | NSS_INIT_FORCEOPEN
         | NSS_INIT_NOROOTINIT | NSS_INIT_OPTIMIZESPACE | NSS_INIT_PK11RELOAD);
  if(nss_context != NULL)
    return CURLE_OK;

  infof(data, "Unable to initialize NSS\n");
  return CURLE_SSL_CACERT_BADFILE;
}
