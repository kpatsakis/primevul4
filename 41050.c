prng_bytes (uint8_t *output, int len)
{
  static size_t processed = 0;

  if (nonce_md)
    {
      const int md_size = md_kt_size (nonce_md);
      while (len > 0)
	{
	  unsigned int outlen = 0;
	  const int blen = min_int (len, md_size);
	  md_full(nonce_md, nonce_data, md_size + nonce_secret_len, nonce_data);
	  memcpy (output, nonce_data, blen);
	  output += blen;
	  len -= blen;

	  /* Ensure that random data is reset regularly */
	  processed += blen;
	  if(processed > PRNG_NONCE_RESET_BYTES) {
	    prng_reset_nonce();
	    processed = 0;
	  }
	}
    }
  else
    rand_bytes (output, len);
}
