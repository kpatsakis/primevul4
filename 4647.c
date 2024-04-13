char *Curl_all_content_encodings(void)
{
  return strdup(CONTENT_ENCODING_DEFAULT);  /* Satisfy caller. */
}