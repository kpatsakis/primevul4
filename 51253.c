void Curl_nss_sha256sum(const unsigned char *tmp, /* input */
                     size_t tmplen,
                     unsigned char *sha256sum, /* output */
                     size_t sha256len)
{
  PK11Context *SHA256pw = PK11_CreateDigestContext(SEC_OID_SHA256);
  unsigned int SHA256out;

  PK11_DigestOp(SHA256pw, tmp, curlx_uztoui(tmplen));
  PK11_DigestFinal(SHA256pw, sha256sum, &SHA256out, curlx_uztoui(sha256len));
  PK11_DestroyContext(SHA256pw, PR_TRUE);
}
