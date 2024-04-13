static void brotli_close_writer(struct Curl_easy *data,
                                struct contenc_writer *writer)
{
  struct brotli_params *bp = (struct brotli_params *) &writer->params;
  (void) data;

  if(bp->br) {
    BrotliDecoderDestroyInstance(bp->br);
    bp->br = NULL;
  }
}