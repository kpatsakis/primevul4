bool Curl_nss_false_start(void) {
#if NSSVERNUM >= 0x030f04 /* 3.15.4 */
  return TRUE;
#else
  return FALSE;
#endif
}
