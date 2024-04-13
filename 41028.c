check_key (struct key *key, const struct key_type *kt)
{
  if (kt->cipher)
    {
      /*
       * Check for zero key
       */
      if (key_is_zero(key, kt))
	return false;

      /*
       * Check for weak or semi-weak DES keys.
       */
      {
	const int ndc = key_des_num_cblocks (kt->cipher);
	if (ndc)
	  return key_des_check (key->cipher, kt->cipher_length, ndc);
	else
	  return true;
      }
    }
  return true;
}
