void lzxd_set_output_length(struct lzxd_stream *lzx, off_t out_bytes) {
  if (lzx) lzx->length = out_bytes;
}
