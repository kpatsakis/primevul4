fixup_key (struct key *key, const struct key_type *kt)
{
  struct gc_arena gc = gc_new ();
  if (kt->cipher)
    {
#ifdef ENABLE_DEBUG
      const struct key orig = *key;
#endif
      const int ndc = key_des_num_cblocks (kt->cipher);

      if (ndc)
	key_des_fixup (key->cipher, kt->cipher_length, ndc);

#ifdef ENABLE_DEBUG
      if (check_debug_level (D_CRYPTO_DEBUG))
	{
	  if (memcmp (orig.cipher, key->cipher, kt->cipher_length))
	    dmsg (D_CRYPTO_DEBUG, "CRYPTO INFO: fixup_key: before=%s after=%s",
		 format_hex (orig.cipher, kt->cipher_length, 0, &gc),
		 format_hex (key->cipher, kt->cipher_length, 0, &gc));
	}
#endif
    }
  gc_free (&gc);
}
