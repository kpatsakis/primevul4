rend_service_validate_intro_late(const rend_intro_cell_t *intro,
                                 char **err_msg_out)
{
  int status = 0;

  if (!intro) {
    if (err_msg_out)
      *err_msg_out =
        tor_strdup("NULL intro cell passed to "
                   "rend_service_validate_intro_late()");

    status = -1;
    goto err;
  }

  if (intro->version == 3 && intro->parsed) {
    if (!(intro->u.v3.auth_type == REND_NO_AUTH ||
          intro->u.v3.auth_type == REND_BASIC_AUTH ||
          intro->u.v3.auth_type == REND_STEALTH_AUTH)) {
      /* This is an informative message, not an error, as in the old code */
      if (err_msg_out)
        tor_asprintf(err_msg_out,
                     "unknown authorization type %d",
                     intro->u.v3.auth_type);
    }
  }

 err:
  return status;
}
