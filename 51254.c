size_t Curl_nss_version(char *buffer, size_t size)
{
  return snprintf(buffer, size, "NSS/%s", NSS_VERSION);
}
