rend_service_begin_parse_intro(const uint8_t *request,
                               size_t request_len,
                               uint8_t type,
                               char **err_msg_out)
{
  rend_intro_cell_t *rv = NULL;
  char *err_msg = NULL;

  if (!request || request_len <= 0) goto err;
  if (!(type == 1 || type == 2)) goto err;

  /* First, check that the cell is long enough to be a sensible INTRODUCE */

  /* min key length plus digest length plus nickname length */
  if (request_len <
        (DIGEST_LEN + REND_COOKIE_LEN + (MAX_NICKNAME_LEN + 1) +
         DH_KEY_LEN + 42)) {
    if (err_msg_out) {
      tor_asprintf(&err_msg,
                   "got a truncated INTRODUCE%d cell",
                   (int)type);
    }
    goto err;
  }

  /* Allocate a new parsed cell structure */
  rv = tor_malloc_zero(sizeof(*rv));

  /* Set the type */
  rv->type = type;

  /* Copy in the ID */
  memcpy(rv->pk, request, DIGEST_LEN);

  /* Copy in the ciphertext */
  rv->ciphertext = tor_malloc(request_len - DIGEST_LEN);
  memcpy(rv->ciphertext, request + DIGEST_LEN, request_len - DIGEST_LEN);
  rv->ciphertext_len = request_len - DIGEST_LEN;

  goto done;

 err:
  rend_service_free_intro(rv);
  rv = NULL;

  if (err_msg_out && !err_msg) {
    tor_asprintf(&err_msg,
                 "unknown INTRODUCE%d error",
                 (int)type);
  }

 done:
  if (err_msg_out) *err_msg_out = err_msg;
  else tor_free(err_msg);

  return rv;
}
