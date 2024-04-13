void Curl_nss_md5sum(unsigned char *tmp, /* input */
                     size_t tmplen,
                     unsigned char *md5sum, /* output */
                     size_t md5len)
{
  PK11Context *MD5pw = PK11_CreateDigestContext(SEC_OID_MD5);
  unsigned int MD5out;

  PK11_DigestOp(MD5pw, tmp, curlx_uztoui(tmplen));
  PK11_DigestFinal(MD5pw, md5sum, &MD5out, curlx_uztoui(md5len));
  PK11_DestroyContext(MD5pw, PR_TRUE);
}
