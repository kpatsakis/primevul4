md5_digest_defined (const struct md5_digest *digest)
{
  int i;
  for (i = 0; i < MD5_DIGEST_LENGTH; ++i)
    if (digest->digest[i])
      return true;
  return false;
}
