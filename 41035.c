generate_key_random (struct key *key, const struct key_type *kt)
{
  int cipher_len = MAX_CIPHER_KEY_LENGTH;
  int hmac_len = MAX_HMAC_KEY_LENGTH;

  struct gc_arena gc = gc_new ();

  do {
    CLEAR (*key);
    if (kt)
      {
	if (kt->cipher && kt->cipher_length > 0 && kt->cipher_length <= cipher_len)
	  cipher_len = kt->cipher_length;

	if (kt->digest && kt->hmac_length > 0 && kt->hmac_length <= hmac_len)
	  hmac_len = kt->hmac_length;
      }
    if (!rand_bytes (key->cipher, cipher_len)
	|| !rand_bytes (key->hmac, hmac_len))
      msg (M_FATAL, "ERROR: Random number generator cannot obtain entropy for key generation");

    dmsg (D_SHOW_KEY_SOURCE, "Cipher source entropy: %s", format_hex (key->cipher, cipher_len, 0, &gc));
    dmsg (D_SHOW_KEY_SOURCE, "HMAC source entropy: %s", format_hex (key->hmac, hmac_len, 0, &gc));

    if (kt)
      fixup_key (key, kt);
  } while (kt && !check_key (key, kt));

  gc_free (&gc);
}
