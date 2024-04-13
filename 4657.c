CURLcode Curl_unencode_write(struct Curl_easy *data,
                             struct contenc_writer *writer,
                             const char *buf, size_t nbytes)
{
  if(!nbytes)
    return CURLE_OK;
  return writer->handler->unencode_write(data, writer, buf, nbytes);
}