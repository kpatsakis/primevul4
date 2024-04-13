CURLcode Curl_nss_force_init(struct Curl_easy *data)
{
  CURLcode result;
  if(!nss_initlock) {
    if(data)
      failf(data, "unable to initialize NSS, curl_global_init() should have "
                  "been called with CURL_GLOBAL_SSL or CURL_GLOBAL_ALL");
    return CURLE_FAILED_INIT;
  }

  PR_Lock(nss_initlock);
  result = nss_init(data);
  PR_Unlock(nss_initlock);

  return result;
}
