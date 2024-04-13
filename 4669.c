static CURLcode identity_unencode_write(struct Curl_easy *data,
                                        struct contenc_writer *writer,
                                        const char *buf, size_t nbytes)
{
  return Curl_unencode_write(data, writer->downstream, buf, nbytes);
}