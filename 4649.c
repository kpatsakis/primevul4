static CURLcode zstd_init_writer(struct Curl_easy *data,
                                 struct contenc_writer *writer)
{
  struct zstd_params *zp = (struct zstd_params *)&writer->params;
  (void)data;

  if(!writer->downstream)
    return CURLE_WRITE_ERROR;

  zp->zds = ZSTD_createDStream();
  zp->decomp = NULL;
  return zp->zds ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}