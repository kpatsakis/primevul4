crypto_adjust_frame_parameters(struct frame *frame,
			       const struct key_type* kt,
			       bool cipher_defined,
			       bool use_iv,
			       bool packet_id,
			       bool packet_id_long_form)
{
  frame_add_to_extra_frame (frame,
			    (packet_id ? packet_id_size (packet_id_long_form) : 0) +
			    ((cipher_defined && use_iv) ? cipher_kt_iv_size (kt->cipher) : 0) +
			    (cipher_defined ? cipher_kt_block_size (kt->cipher) : 0) + /* worst case padding expansion */
			    kt->hmac_length);
}
