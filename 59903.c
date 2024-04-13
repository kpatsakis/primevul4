rend_service_parse_intro_for_v0_or_v1(
    rend_intro_cell_t *intro,
    const uint8_t *buf,
    size_t plaintext_len,
    char **err_msg_out)
{
  const char *rp_nickname, *endptr;
  size_t nickname_field_len, ver_specific_len;

  if (intro->version == 1) {
    ver_specific_len = MAX_HEX_NICKNAME_LEN + 2;
    rp_nickname = ((const char *)buf) + 1;
    nickname_field_len = MAX_HEX_NICKNAME_LEN + 1;
  } else if (intro->version == 0) {
    ver_specific_len = MAX_NICKNAME_LEN + 1;
    rp_nickname = (const char *)buf;
    nickname_field_len = MAX_NICKNAME_LEN + 1;
  } else {
    if (err_msg_out)
      tor_asprintf(err_msg_out,
                   "rend_service_parse_intro_for_v0_or_v1() called with "
                   "bad version %d on INTRODUCE%d cell (this is a bug)",
                   intro->version,
                   (int)(intro->type));
    goto err;
  }

  if (plaintext_len < ver_specific_len) {
    if (err_msg_out)
      tor_asprintf(err_msg_out,
                   "short plaintext of encrypted part in v1 INTRODUCE%d "
                   "cell (%lu bytes, needed %lu)",
                   (int)(intro->type),
                   (unsigned long)plaintext_len,
                   (unsigned long)ver_specific_len);
    goto err;
  }

  endptr = memchr(rp_nickname, 0, nickname_field_len);
  if (!endptr || endptr == rp_nickname) {
    if (err_msg_out) {
      tor_asprintf(err_msg_out,
                   "couldn't find a nul-padded nickname in "
                   "INTRODUCE%d cell",
                   (int)(intro->type));
    }
    goto err;
  }

  if ((intro->version == 0 &&
       !is_legal_nickname(rp_nickname)) ||
      (intro->version == 1 &&
       !is_legal_nickname_or_hexdigest(rp_nickname))) {
    if (err_msg_out) {
      tor_asprintf(err_msg_out,
                   "bad nickname in INTRODUCE%d cell",
                   (int)(intro->type));
    }
    goto err;
  }

  memcpy(intro->u.v0_v1.rp, rp_nickname, endptr - rp_nickname + 1);

  return ver_specific_len;

 err:
  return -1;
}
