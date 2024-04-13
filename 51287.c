static SECStatus set_ciphers(struct Curl_easy *data, PRFileDesc * model,
                             char *cipher_list)
{
  unsigned int i;
  PRBool cipher_state[NUM_OF_CIPHERS];
  PRBool found;
  char *cipher;

  /* use accessors to avoid dynamic linking issues after an update of NSS */
  const PRUint16 num_implemented_ciphers = SSL_GetNumImplementedCiphers();
  const PRUint16 *implemented_ciphers = SSL_GetImplementedCiphers();
  if(!implemented_ciphers)
    return SECFailure;

  /* First disable all ciphers. This uses a different max value in case
   * NSS adds more ciphers later we don't want them available by
   * accident
   */
  for(i = 0; i < num_implemented_ciphers; i++) {
    SSL_CipherPrefSet(model, implemented_ciphers[i], PR_FALSE);
  }

  /* Set every entry in our list to false */
  for(i = 0; i < NUM_OF_CIPHERS; i++) {
    cipher_state[i] = PR_FALSE;
  }

  cipher = cipher_list;

  while(cipher_list && (cipher_list[0])) {
    while((*cipher) && (ISSPACE(*cipher)))
      ++cipher;

    if((cipher_list = strchr(cipher, ','))) {
      *cipher_list++ = '\0';
    }

    found = PR_FALSE;

    for(i=0; i<NUM_OF_CIPHERS; i++) {
      if(Curl_raw_equal(cipher, cipherlist[i].name)) {
        cipher_state[i] = PR_TRUE;
        found = PR_TRUE;
        break;
      }
    }

    if(found == PR_FALSE) {
      failf(data, "Unknown cipher in list: %s", cipher);
      return SECFailure;
    }

    if(cipher_list) {
      cipher = cipher_list;
    }
  }

  /* Finally actually enable the selected ciphers */
  for(i=0; i<NUM_OF_CIPHERS; i++) {
    if(!cipher_state[i])
      continue;

    if(SSL_CipherPrefSet(model, cipherlist[i].num, PR_TRUE) != SECSuccess) {
      failf(data, "cipher-suite not supported by NSS: %s", cipherlist[i].name);
      return SECFailure;
    }
  }

  return SECSuccess;
}
