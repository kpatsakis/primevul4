static CURLcode error_init_writer(struct Curl_easy *data,
                                  struct contenc_writer *writer)
{
  (void) data;
  return writer->downstream? CURLE_OK: CURLE_WRITE_ERROR;
}