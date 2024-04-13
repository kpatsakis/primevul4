static CURLcode brotli_init_writer(struct Curl_easy *data,
                                   struct contenc_writer *writer)
{
  struct brotli_params *bp = (struct brotli_params *) &writer->params;
  (void) data;

  if(!writer->downstream)
    return CURLE_WRITE_ERROR;

  bp->br = BrotliDecoderCreateInstance(NULL, NULL, NULL);
  return bp->br? CURLE_OK: CURLE_OUT_OF_MEMORY;
}