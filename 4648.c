static CURLcode client_unencode_write(struct Curl_easy *data,
                                      struct contenc_writer *writer,
                                      const char *buf, size_t nbytes)
{
  struct SingleRequest *k = &data->req;

  (void) writer;

  if(!nbytes || k->ignorebody)
    return CURLE_OK;

  return Curl_client_write(data, CLIENTWRITE_BODY, (char *) buf, nbytes);
}