prng_init (const char *md_name, const int nonce_secret_len_parm)
{
  prng_uninit ();
  nonce_md = md_name ? md_kt_get (md_name) : NULL;
  if (nonce_md)
    {
      ASSERT (nonce_secret_len_parm >= NONCE_SECRET_LEN_MIN && nonce_secret_len_parm <= NONCE_SECRET_LEN_MAX);
      nonce_secret_len = nonce_secret_len_parm;
      {
	const int size = md_kt_size(nonce_md) + nonce_secret_len;
	dmsg (D_CRYPTO_DEBUG, "PRNG init md=%s size=%d", md_kt_name(nonce_md), size);
	nonce_data = (uint8_t*) malloc (size);
	check_malloc_return (nonce_data);
	prng_reset_nonce();
      }
    }
}
