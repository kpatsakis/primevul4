static CURLcode nss_init_sslver(SSLVersionRange *sslver,
                                struct Curl_easy *data)
{
  switch(data->set.ssl.version) {
  default:
  case CURL_SSLVERSION_DEFAULT:
  case CURL_SSLVERSION_TLSv1:
    sslver->min = SSL_LIBRARY_VERSION_TLS_1_0;
#ifdef SSL_LIBRARY_VERSION_TLS_1_2
    sslver->max = SSL_LIBRARY_VERSION_TLS_1_2;
#elif defined SSL_LIBRARY_VERSION_TLS_1_1
    sslver->max = SSL_LIBRARY_VERSION_TLS_1_1;
#else
    sslver->max = SSL_LIBRARY_VERSION_TLS_1_0;
#endif
    return CURLE_OK;

  case CURL_SSLVERSION_SSLv2:
    sslver->min = SSL_LIBRARY_VERSION_2;
    sslver->max = SSL_LIBRARY_VERSION_2;
    return CURLE_OK;

  case CURL_SSLVERSION_SSLv3:
    sslver->min = SSL_LIBRARY_VERSION_3_0;
    sslver->max = SSL_LIBRARY_VERSION_3_0;
    return CURLE_OK;

  case CURL_SSLVERSION_TLSv1_0:
    sslver->min = SSL_LIBRARY_VERSION_TLS_1_0;
    sslver->max = SSL_LIBRARY_VERSION_TLS_1_0;
    return CURLE_OK;

  case CURL_SSLVERSION_TLSv1_1:
#ifdef SSL_LIBRARY_VERSION_TLS_1_1
    sslver->min = SSL_LIBRARY_VERSION_TLS_1_1;
    sslver->max = SSL_LIBRARY_VERSION_TLS_1_1;
    return CURLE_OK;
#endif
    break;

  case CURL_SSLVERSION_TLSv1_2:
#ifdef SSL_LIBRARY_VERSION_TLS_1_2
    sslver->min = SSL_LIBRARY_VERSION_TLS_1_2;
    sslver->max = SSL_LIBRARY_VERSION_TLS_1_2;
    return CURLE_OK;
#endif
    break;
  }

  failf(data, "TLS minor version cannot be set");
  return CURLE_SSL_CONNECT_ERROR;
}
