find_rp_for_intro(const rend_intro_cell_t *intro,
                  char **err_msg_out)
{
  extend_info_t *rp = NULL;
  char *err_msg = NULL;
  const char *rp_nickname = NULL;
  const node_t *node = NULL;

  if (!intro) {
    if (err_msg_out)
      err_msg = tor_strdup("Bad parameters to find_rp_for_intro()");

    goto err;
  }

  if (intro->version == 0 || intro->version == 1) {
    rp_nickname = (const char *)(intro->u.v0_v1.rp);

    node = node_get_by_nickname(rp_nickname, 0);
    if (!node) {
      if (err_msg_out) {
        tor_asprintf(&err_msg,
                     "Couldn't find router %s named in INTRODUCE2 cell",
                     escaped_safe_str_client(rp_nickname));
      }

      goto err;
    }

    /* Are we in single onion mode? */
    const int allow_direct = rend_service_allow_non_anonymous_connection(
                                                                get_options());
    rp = extend_info_from_node(node, allow_direct);
    if (!rp) {
      if (err_msg_out) {
        tor_asprintf(&err_msg,
                     "Couldn't build extend_info_t for router %s named "
                     "in INTRODUCE2 cell",
                     escaped_safe_str_client(rp_nickname));
      }

      goto err;
    }
  } else if (intro->version == 2) {
    rp = extend_info_dup(intro->u.v2.extend_info);
  } else if (intro->version == 3) {
    rp = extend_info_dup(intro->u.v3.extend_info);
  } else {
    if (err_msg_out) {
      tor_asprintf(&err_msg,
                   "Unknown version %d in INTRODUCE2 cell",
                   (int)(intro->version));
    }

    goto err;
  }

  /* rp is always set here: extend_info_dup guarantees a non-NULL result, and
   * the other cases goto err. */
  tor_assert(rp);

  /* Make sure the RP we are being asked to connect to is _not_ a private
   * address unless it's allowed. Let's avoid to build a circuit to our
   * second middle node and fail right after when extending to the RP. */
  if (!extend_info_addr_is_allowed(&rp->addr)) {
    if (err_msg_out) {
      tor_asprintf(&err_msg,
                   "Relay IP in INTRODUCE2 cell is private address.");
    }
    extend_info_free(rp);
    rp = NULL;
    goto err;
  }
  goto done;

 err:
  if (err_msg_out)
    *err_msg_out = err_msg;
  else
    tor_free(err_msg);

 done:
  return rp;
}
