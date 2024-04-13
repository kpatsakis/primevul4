free_ssl_lib (void)
{
  crypto_uninit_lib ();
  prng_uninit();
}
