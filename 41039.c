key2_print (const struct key2* k,
	    const struct key_type *kt,
	    const char* prefix0,
	    const char* prefix1)
{
  struct gc_arena gc = gc_new ();
  ASSERT (k->n == 2);
  dmsg (D_SHOW_KEY_SOURCE, "%s (cipher): %s",
       prefix0,
       format_hex (k->keys[0].cipher, kt->cipher_length, 0, &gc));
  dmsg (D_SHOW_KEY_SOURCE, "%s (hmac): %s",
       prefix0,
       format_hex (k->keys[0].hmac, kt->hmac_length, 0, &gc));
  dmsg (D_SHOW_KEY_SOURCE, "%s (cipher): %s",
       prefix1,
       format_hex (k->keys[1].cipher, kt->cipher_length, 0, &gc));
  dmsg (D_SHOW_KEY_SOURCE, "%s (hmac): %s",
       prefix1,
       format_hex (k->keys[1].hmac, kt->hmac_length, 0, &gc));
  gc_free (&gc);
}
