int Curl_nss_seed(struct Curl_easy *data)
{
  /* make sure that NSS is initialized */
  return !!Curl_nss_force_init(data);
}
