static CURLcode error_unencode_write(struct Curl_easy *data,
                                     struct contenc_writer *writer,
                                     const char *buf, size_t nbytes)
{
  char *all = Curl_all_content_encodings();

  (void) writer;
  (void) buf;
  (void) nbytes;

  if(!all)
    return CURLE_OUT_OF_MEMORY;
  failf(data, "Unrecognized content encoding type. "
        "libcurl understands %s content encodings.", all);
  free(all);
  return CURLE_BAD_CONTENT_ENCODING;
}