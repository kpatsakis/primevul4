write_key (const struct key *key, const struct key_type *kt,
	   struct buffer *buf)
{
  ASSERT (kt->cipher_length <= MAX_CIPHER_KEY_LENGTH
	  && kt->hmac_length <= MAX_HMAC_KEY_LENGTH);

  if (!buf_write (buf, &kt->cipher_length, 1))
    return false;
  if (!buf_write (buf, &kt->hmac_length, 1))
    return false;
  if (!buf_write (buf, key->cipher, kt->cipher_length))
    return false;
  if (!buf_write (buf, key->hmac, kt->hmac_length))
    return false;

  return true;
}
