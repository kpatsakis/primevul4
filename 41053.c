prng_uninit (void)
{
  free (nonce_data);
  nonce_data = NULL;
  nonce_md = NULL;
  nonce_secret_len = 0;
}
