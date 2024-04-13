rend_service_free_intro(rend_intro_cell_t *request)
{
  if (!request) {
    return;
  }

  /* Free ciphertext */
  tor_free(request->ciphertext);
  request->ciphertext_len = 0;

  /* Have plaintext? */
  if (request->plaintext) {
    /* Zero it out just to be safe */
    memwipe(request->plaintext, 0, request->plaintext_len);
    tor_free(request->plaintext);
    request->plaintext_len = 0;
  }

  /* Have parsed plaintext? */
  if (request->parsed) {
    switch (request->version) {
      case 0:
      case 1:
        /*
         * Nothing more to do; these formats have no further pointers
         * in them.
         */
        break;
      case 2:
        extend_info_free(request->u.v2.extend_info);
        request->u.v2.extend_info = NULL;
        break;
      case 3:
        if (request->u.v3.auth_data) {
          memwipe(request->u.v3.auth_data, 0, request->u.v3.auth_len);
          tor_free(request->u.v3.auth_data);
        }

        extend_info_free(request->u.v3.extend_info);
        request->u.v3.extend_info = NULL;
        break;
      default:
        log_info(LD_BUG,
                 "rend_service_free_intro() saw unknown protocol "
                 "version %d.",
                 request->version);
    }
  }

  /* Zero it out to make sure sensitive stuff doesn't hang around in memory */
  memwipe(request, 0, sizeof(*request));

  tor_free(request);
}
