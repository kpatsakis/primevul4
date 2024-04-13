md5_digest_equal (const struct md5_digest *d1, const struct md5_digest *d2)
{
  return memcmp(d1->digest, d2->digest, MD5_DIGEST_LENGTH) == 0;
}
