 openvpn_encrypt (struct buffer *buf, struct buffer work,
 		 const struct crypto_options *opt,
		 const struct frame* frame)
{
  struct gc_arena gc;
  gc_init (&gc);

  if (buf->len > 0 && opt->key_ctx_bi)
    {
      struct key_ctx *ctx = &opt->key_ctx_bi->encrypt;

      /* Do Encrypt from buf -> work */
      if (ctx->cipher)
	{
	  uint8_t iv_buf[OPENVPN_MAX_IV_LENGTH];
	  const int iv_size = cipher_ctx_iv_length (ctx->cipher);
	  const unsigned int mode = cipher_ctx_mode (ctx->cipher);
	  int outlen;

	  if (mode == OPENVPN_MODE_CBC)
	    {
	      CLEAR (iv_buf);

	      /* generate pseudo-random IV */
	      if (opt->flags & CO_USE_IV)
		prng_bytes (iv_buf, iv_size);

	      /* Put packet ID in plaintext buffer or IV, depending on cipher mode */
	      if (opt->packet_id)
		{
		  struct packet_id_net pin;
		  packet_id_alloc_outgoing (&opt->packet_id->send, &pin, BOOL_CAST (opt->flags & CO_PACKET_ID_LONG_FORM));
		  ASSERT (packet_id_write (&pin, buf, BOOL_CAST (opt->flags & CO_PACKET_ID_LONG_FORM), true));
		}
	    }
	  else if (mode == OPENVPN_MODE_CFB || mode == OPENVPN_MODE_OFB)
	    {
	      struct packet_id_net pin;
	      struct buffer b;

	      ASSERT (opt->flags & CO_USE_IV);    /* IV and packet-ID required */
	      ASSERT (opt->packet_id); /*  for this mode. */

	      packet_id_alloc_outgoing (&opt->packet_id->send, &pin, true);
	      memset (iv_buf, 0, iv_size);
	      buf_set_write (&b, iv_buf, iv_size);
	      ASSERT (packet_id_write (&pin, &b, true, false));
	    }
	  else /* We only support CBC, CFB, or OFB modes right now */
	    {
	      ASSERT (0);
	    }

	  /* initialize work buffer with FRAME_HEADROOM bytes of prepend capacity */
	  ASSERT (buf_init (&work, FRAME_HEADROOM (frame)));

	  /* set the IV pseudo-randomly */
	  if (opt->flags & CO_USE_IV)
	    dmsg (D_PACKET_CONTENT, "ENCRYPT IV: %s", format_hex (iv_buf, iv_size, 0, &gc));

	  dmsg (D_PACKET_CONTENT, "ENCRYPT FROM: %s",
	       format_hex (BPTR (buf), BLEN (buf), 80, &gc));

	  /* cipher_ctx was already initialized with key & keylen */
	  ASSERT (cipher_ctx_reset(ctx->cipher, iv_buf));

	  /* Buffer overflow check */
	  if (!buf_safe (&work, buf->len + cipher_ctx_block_size(ctx->cipher)))
	    {
	      msg (D_CRYPT_ERRORS, "ENCRYPT: buffer size error, bc=%d bo=%d bl=%d wc=%d wo=%d wl=%d cbs=%d",
		   buf->capacity,
		   buf->offset,
		   buf->len,
		   work.capacity,
		   work.offset,
		   work.len,
		   cipher_ctx_block_size (ctx->cipher));
	      goto err;
	    }

	  /* Encrypt packet ID, payload */
	  ASSERT (cipher_ctx_update (ctx->cipher, BPTR (&work), &outlen, BPTR (buf), BLEN (buf)));
	  work.len += outlen;

	  /* Flush the encryption buffer */
	  ASSERT(cipher_ctx_final(ctx->cipher, BPTR (&work) + outlen, &outlen));
	  work.len += outlen;
	  ASSERT (outlen == iv_size);

	  /* prepend the IV to the ciphertext */
	  if (opt->flags & CO_USE_IV)
	    {
	      uint8_t *output = buf_prepend (&work, iv_size);
	      ASSERT (output);
	      memcpy (output, iv_buf, iv_size);
	    }

	  dmsg (D_PACKET_CONTENT, "ENCRYPT TO: %s",
	       format_hex (BPTR (&work), BLEN (&work), 80, &gc));
	}
      else				/* No Encryption */
	{
	  if (opt->packet_id)
	    {
	      struct packet_id_net pin;
	      packet_id_alloc_outgoing (&opt->packet_id->send, &pin, BOOL_CAST (opt->flags & CO_PACKET_ID_LONG_FORM));
	      ASSERT (packet_id_write (&pin, buf, BOOL_CAST (opt->flags & CO_PACKET_ID_LONG_FORM), true));
	    }
	  work = *buf;
	}

      /* HMAC the ciphertext (or plaintext if !cipher) */
      if (ctx->hmac)
	{
	  uint8_t *output = NULL;

	  hmac_ctx_reset (ctx->hmac);
	  hmac_ctx_update (ctx->hmac, BPTR(&work), BLEN(&work));
	  output = buf_prepend (&work, hmac_ctx_size(ctx->hmac));
	  ASSERT (output);
	  hmac_ctx_final (ctx->hmac, output);
	}

      *buf = work;
    }

  gc_free (&gc);
  return;

err:
  crypto_clear_error();
  buf->len = 0;
  gc_free (&gc);
  return;
}
