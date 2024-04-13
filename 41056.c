test_crypto (const struct crypto_options *co, struct frame* frame)
{
  int i, j;
  struct gc_arena gc = gc_new ();
  struct buffer src = alloc_buf_gc (TUN_MTU_SIZE (frame), &gc);
  struct buffer work = alloc_buf_gc (BUF_SIZE (frame), &gc);
  struct buffer encrypt_workspace = alloc_buf_gc (BUF_SIZE (frame), &gc);
  struct buffer decrypt_workspace = alloc_buf_gc (BUF_SIZE (frame), &gc);
  struct buffer buf = clear_buf();

  /* init work */
  ASSERT (buf_init (&work, FRAME_HEADROOM (frame)));

  msg (M_INFO, "Entering " PACKAGE_NAME " crypto self-test mode.");
  for (i = 1; i <= TUN_MTU_SIZE (frame); ++i)
    {
      update_time ();

      msg (M_INFO, "TESTING ENCRYPT/DECRYPT of packet length=%d", i);

      /*
       * Load src with random data.
       */
      ASSERT (buf_init (&src, 0));
      ASSERT (i <= src.capacity);
      src.len = i;
      ASSERT (rand_bytes (BPTR (&src), BLEN (&src)));

      /* copy source to input buf */
      buf = work;
      memcpy (buf_write_alloc (&buf, BLEN (&src)), BPTR (&src), BLEN (&src));

      /* encrypt */
      openvpn_encrypt (&buf, encrypt_workspace, co, frame);

      /* decrypt */
      openvpn_decrypt (&buf, decrypt_workspace, co, frame);

      /* compare */
      if (buf.len != src.len)
	msg (M_FATAL, "SELF TEST FAILED, src.len=%d buf.len=%d", src.len, buf.len);
      for (j = 0; j < i; ++j)
	{
	  const uint8_t in = *(BPTR (&src) + j);
	  const uint8_t out = *(BPTR (&buf) + j);
	  if (in != out)
	    msg (M_FATAL, "SELF TEST FAILED, pos=%d in=%d out=%d", j, in, out);
	}
    }
  msg (M_INFO, PACKAGE_NAME " crypto self-test mode SUCCEEDED.");
  gc_free (&gc);
}
