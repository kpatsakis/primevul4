key_is_zero (struct key *key, const struct key_type *kt)
{
  int i;
  for (i = 0; i < kt->cipher_length; ++i)
    if (key->cipher[i])
      return false;
  msg (D_CRYPT_ERRORS, "CRYPTO INFO: WARNING: zero key detected");
  return true;
}
