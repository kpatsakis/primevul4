static void zstd_close_writer(struct Curl_easy *data,
                              struct contenc_writer *writer)
{
  struct zstd_params *zp = (struct zstd_params *)&writer->params;
  (void)data;

  if(zp->decomp) {
    free(zp->decomp);
    zp->decomp = NULL;
  }
  if(zp->zds) {
    ZSTD_freeDStream(zp->zds);
    zp->zds = NULL;
  }
}