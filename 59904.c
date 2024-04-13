rend_service_parse_intro_plaintext(
    rend_intro_cell_t *intro,
    char **err_msg_out)
{
  char *err_msg = NULL;
  ssize_t ver_specific_len, ver_invariant_len;
  uint8_t version;
  int status = -1;

  if (!intro) {
    if (err_msg_out) {
      err_msg =
        tor_strdup("rend_service_parse_intro_plaintext() called with NULL "
                   "rend_intro_cell_t");
    }

    status = -2;
    goto err;
  }

  /* Check that we have plaintext */
  if (!(intro->plaintext) || intro->plaintext_len <= 0) {
    if (err_msg_out) {
      err_msg = tor_strdup("rend_intro_cell_t was missing plaintext");
    }
    status = -3;
    goto err;
  }

  /* In all formats except v0, the first byte is a version number */
  version = intro->plaintext[0];

  /* v0 has no version byte (stupid...), so handle it as a fallback */
  if (version > 3) version = 0;

  /* Copy the version into the parsed cell structure */
  intro->version = version;

  /* Call the version-specific parser from the table */
  ver_specific_len =
    intro_version_handlers[version](intro,
                                    intro->plaintext, intro->plaintext_len,
                                    &err_msg);
  if (ver_specific_len < 0) {
    status = -4;
    goto err;
  }

  /** The rendezvous cookie and Diffie-Hellman stuff are version-invariant
   * and at the end of the plaintext of the encrypted part of the cell.
   */

  ver_invariant_len = intro->plaintext_len - ver_specific_len;
  if (ver_invariant_len < REND_COOKIE_LEN + DH_KEY_LEN) {
    tor_asprintf(&err_msg,
        "decrypted plaintext of INTRODUCE%d cell was truncated (%ld bytes)",
        (int)(intro->type),
        (long)(intro->plaintext_len));
    status = -5;
    goto err;
  } else if (ver_invariant_len > REND_COOKIE_LEN + DH_KEY_LEN) {
    tor_asprintf(&err_msg,
        "decrypted plaintext of INTRODUCE%d cell was too long (%ld bytes)",
        (int)(intro->type),
        (long)(intro->plaintext_len));
    status = -6;
    goto err;
  } else {
    memcpy(intro->rc,
           intro->plaintext + ver_specific_len,
           REND_COOKIE_LEN);
    memcpy(intro->dh,
           intro->plaintext + ver_specific_len + REND_COOKIE_LEN,
           DH_KEY_LEN);
  }

  /* Flag it as being fully parsed */
  intro->parsed = 1;

  status = 0;
  goto done;

 err:
  if (err_msg_out && !err_msg) {
    tor_asprintf(&err_msg,
                 "unknown INTRODUCE%d error parsing encrypted part",
                 intro ? (int)(intro->type) : -1);
  }

 done:
  if (err_msg_out) *err_msg_out = err_msg;
  else tor_free(err_msg);

  return status;
}
