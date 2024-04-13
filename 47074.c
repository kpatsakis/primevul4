static void lzxd_reset_state(struct lzxd_stream *lzx) {
  int i;

  lzx->R0              = 1;
  lzx->R1              = 1;
  lzx->R2              = 1;
  lzx->header_read     = 0;
  lzx->block_remaining = 0;
  lzx->block_type      = LZX_BLOCKTYPE_INVALID;

  /* initialise tables to 0 (because deltas will be applied to them) */
  for (i = 0; i < LZX_MAINTREE_MAXSYMBOLS; i++) lzx->MAINTREE_len[i] = 0;
  for (i = 0; i < LZX_LENGTH_MAXSYMBOLS; i++)   lzx->LENGTH_len[i]   = 0;
}
