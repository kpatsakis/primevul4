static CURLcode zstd_unencode_write(struct Curl_easy *data,
                                    struct contenc_writer *writer,
                                    const char *buf, size_t nbytes)
{
  CURLcode result = CURLE_OK;
  struct zstd_params *zp = (struct zstd_params *)&writer->params;
  ZSTD_inBuffer in;
  ZSTD_outBuffer out;
  size_t errorCode;

  if(!zp->decomp) {
    zp->decomp = malloc(DSIZ);
    if(!zp->decomp)
      return CURLE_OUT_OF_MEMORY;
  }
  in.pos = 0;
  in.src = buf;
  in.size = nbytes;

  for(;;) {
    out.pos = 0;
    out.dst = zp->decomp;
    out.size = DSIZ;

    errorCode = ZSTD_decompressStream(zp->zds, &out, &in);
    if(ZSTD_isError(errorCode)) {
      return CURLE_BAD_CONTENT_ENCODING;
    }
    if(out.pos > 0) {
      result = Curl_unencode_write(data, writer->downstream,
                                   zp->decomp, out.pos);
      if(result)
        break;
    }
    if((in.pos == nbytes) && (out.pos < out.size))
      break;
  }

  return result;
}