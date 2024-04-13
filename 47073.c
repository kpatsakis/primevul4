struct lzxd_stream *lzxd_init(struct mspack_system *system,
			      struct mspack_file *input,
			      struct mspack_file *output,
			      int window_bits,
			      int reset_interval,
			      int input_buffer_size,
			      off_t output_length,
			      char is_delta)
{
  unsigned int window_size = 1 << window_bits;
  struct lzxd_stream *lzx;

  if (!system) return NULL;

  /* LZX DELTA window sizes are between 2^17 (128KiB) and 2^25 (32MiB),
   * regular LZX windows are between 2^15 (32KiB) and 2^21 (2MiB)
   */
  if (is_delta) {
      if (window_bits < 17 || window_bits > 25) return NULL;
  }
  else {
      if (window_bits < 15 || window_bits > 21) return NULL;
  }

  input_buffer_size = (input_buffer_size + 1) & -2;
  if (!input_buffer_size) return NULL;

  /* allocate decompression state */
  if (!(lzx = (struct lzxd_stream *) system->alloc(system, sizeof(struct lzxd_stream)))) {
    return NULL;
  }

  /* allocate decompression window and input buffer */
  lzx->window = (unsigned char *) system->alloc(system, (size_t) window_size);
  lzx->inbuf  = (unsigned char *) system->alloc(system, (size_t) input_buffer_size);
  if (!lzx->window || !lzx->inbuf) {
    system->free(lzx->window);
    system->free(lzx->inbuf);
    system->free(lzx);
    return NULL;
  }

  /* initialise decompression state */
  lzx->sys             = system;
  lzx->input           = input;
  lzx->output          = output;
  lzx->offset          = 0;
  lzx->length          = output_length;

  lzx->inbuf_size      = input_buffer_size;
  lzx->window_size     = 1 << window_bits;
  lzx->ref_data_size   = 0;
  lzx->window_posn     = 0;
  lzx->frame_posn      = 0;
  lzx->frame           = 0;
  lzx->reset_interval  = reset_interval;
  lzx->intel_filesize  = 0;
  lzx->intel_curpos    = 0;
  lzx->intel_started   = 0;
  lzx->error           = MSPACK_ERR_OK;
  lzx->num_offsets     = position_slots[window_bits - 15] << 3;
  lzx->is_delta        = is_delta;

  lzx->o_ptr = lzx->o_end = &lzx->e8_buf[0];
  lzxd_reset_state(lzx);
  INIT_BITS;
  return lzx;
}
