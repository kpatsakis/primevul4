process_zlib_error(struct Curl_easy *data, z_stream *z)
{
  if(z->msg)
    failf(data, "Error while processing content unencoding: %s",
          z->msg);
  else
    failf(data, "Error while processing content unencoding: "
          "Unknown failure within decompression software.");

  return CURLE_BAD_CONTENT_ENCODING;
}