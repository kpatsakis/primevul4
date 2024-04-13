verify_fix_key2 (struct key2 *key2, const struct key_type *kt, const char *shared_secret_file)
{
  int i;

  for (i = 0; i < key2->n; ++i)
    {
      /* Fix parity for DES keys and make sure not a weak key */
      fixup_key (&key2->keys[i], kt);

      /* This should be a very improbable failure */
      if (!check_key (&key2->keys[i], kt))
	msg (M_FATAL, "Key #%d in '%s' is bad.  Try making a new key with --genkey.",
	     i+1, shared_secret_file);
    }
}
