static CURLcode brotli_unencode_write(struct Curl_easy *data,
                                      struct contenc_writer *writer,
                                      const char *buf, size_t nbytes)
{
  struct brotli_params *bp = (struct brotli_params *) &writer->params;
  const uint8_t *src = (const uint8_t *) buf;
  char *decomp;
  uint8_t *dst;
  size_t dstleft;
  CURLcode result = CURLE_OK;
  BrotliDecoderResult r = BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT;

  if(!bp->br)
    return CURLE_WRITE_ERROR;  /* Stream already ended. */

  decomp = malloc(DSIZ);
  if(!decomp)
    return CURLE_OUT_OF_MEMORY;

  while((nbytes || r == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) &&
        result == CURLE_OK) {
    dst = (uint8_t *) decomp;
    dstleft = DSIZ;
    r = BrotliDecoderDecompressStream(bp->br,
                                      &nbytes, &src, &dstleft, &dst, NULL);
    result = Curl_unencode_write(data, writer->downstream,
                                 decomp, DSIZ - dstleft);
    if(result)
      break;
    switch(r) {
    case BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT:
    case BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT:
      break;
    case BROTLI_DECODER_RESULT_SUCCESS:
      BrotliDecoderDestroyInstance(bp->br);
      bp->br = NULL;
      if(nbytes)
        result = CURLE_WRITE_ERROR;
      break;
    default:
      result = brotli_map_error(BrotliDecoderGetErrorCode(bp->br));
      break;
    }
  }
  free(decomp);
  return result;
}