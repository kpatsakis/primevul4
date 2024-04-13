static CURLcode process_trailer(struct Curl_easy *data,
                                struct zlib_params *zp)
{
  z_stream *z = &zp->z;
  CURLcode result = CURLE_OK;
  uInt len = z->avail_in < zp->trailerlen? z->avail_in: zp->trailerlen;

  /* Consume expected trailer bytes. Terminate stream if exhausted.
     Issue an error if unexpected bytes follow. */

  zp->trailerlen -= len;
  z->avail_in -= len;
  z->next_in += len;
  if(z->avail_in)
    result = CURLE_WRITE_ERROR;
  if(result || !zp->trailerlen)
    result = exit_zlib(data, z, &zp->zlib_init, result);
  else {
    /* Only occurs for gzip with zlib < 1.2.0.4 or raw deflate. */
    zp->zlib_init = ZLIB_EXTERNAL_TRAILER;
  }
  return result;
}