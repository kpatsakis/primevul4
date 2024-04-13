rend_service_load_auth_keys(rend_service_t *s, const char *hfname)
{
  int r = 0;
  char *cfname = NULL;
  char *client_keys_str = NULL;
  strmap_t *parsed_clients = strmap_new();
  FILE *cfile, *hfile;
  open_file_t *open_cfile = NULL, *open_hfile = NULL;
  char desc_cook_out[3*REND_DESC_COOKIE_LEN_BASE64+1];
  char service_id[16+1];
  char buf[1500];

  /* Load client keys and descriptor cookies, if available. */
  cfname = rend_service_path(s, client_keys_fname);
  client_keys_str = read_file_to_str(cfname, RFTS_IGNORE_MISSING, NULL);
  if (client_keys_str) {
    if (rend_parse_client_keys(parsed_clients, client_keys_str) < 0) {
      log_warn(LD_CONFIG, "Previously stored client_keys file could not "
               "be parsed.");
      goto err;
    } else {
      log_info(LD_CONFIG, "Parsed %d previously stored client entries.",
               strmap_size(parsed_clients));
    }
  }

  /* Prepare client_keys and hostname files. */
  if (!(cfile = start_writing_to_stdio_file(cfname,
                                            OPEN_FLAGS_REPLACE | O_TEXT,
                                            0600, &open_cfile))) {
    log_warn(LD_CONFIG, "Could not open client_keys file %s",
             escaped(cfname));
    goto err;
  }

  if (!(hfile = start_writing_to_stdio_file(hfname,
                                            OPEN_FLAGS_REPLACE | O_TEXT,
                                            0600, &open_hfile))) {
    log_warn(LD_CONFIG, "Could not open hostname file %s", escaped(hfname));
    goto err;
  }

  /* Either use loaded keys for configured clients or generate new
   * ones if a client is new. */
  SMARTLIST_FOREACH_BEGIN(s->clients, rend_authorized_client_t *, client) {
    rend_authorized_client_t *parsed =
      strmap_get(parsed_clients, client->client_name);
    int written;
    size_t len;
    /* Copy descriptor cookie from parsed entry or create new one. */
    if (parsed) {
      memcpy(client->descriptor_cookie, parsed->descriptor_cookie,
             REND_DESC_COOKIE_LEN);
    } else {
      crypto_rand((char *) client->descriptor_cookie, REND_DESC_COOKIE_LEN);
    }
    /* For compatibility with older tor clients, this does not
     * truncate the padding characters, unlike rend_auth_encode_cookie.  */
    if (base64_encode(desc_cook_out, 3*REND_DESC_COOKIE_LEN_BASE64+1,
                      (char *) client->descriptor_cookie,
                      REND_DESC_COOKIE_LEN, 0) < 0) {
      log_warn(LD_BUG, "Could not base64-encode descriptor cookie.");
      goto err;
    }
    /* Copy client key from parsed entry or create new one if required. */
    if (parsed && parsed->client_key) {
      client->client_key = crypto_pk_dup_key(parsed->client_key);
    } else if (s->auth_type == REND_STEALTH_AUTH) {
      /* Create private key for client. */
      crypto_pk_t *prkey = NULL;
      if (!(prkey = crypto_pk_new())) {
        log_warn(LD_BUG,"Error constructing client key");
        goto err;
      }
      if (crypto_pk_generate_key(prkey)) {
        log_warn(LD_BUG,"Error generating client key");
        crypto_pk_free(prkey);
        goto err;
      }
      if (crypto_pk_check_key(prkey) <= 0) {
        log_warn(LD_BUG,"Generated client key seems invalid");
        crypto_pk_free(prkey);
        goto err;
      }
      client->client_key = prkey;
    }
    /* Add entry to client_keys file. */
    written = tor_snprintf(buf, sizeof(buf),
                           "client-name %s\ndescriptor-cookie %s\n",
                           client->client_name, desc_cook_out);
    if (written < 0) {
      log_warn(LD_BUG, "Could not write client entry.");
      goto err;
    }
    if (client->client_key) {
      char *client_key_out = NULL;
      if (crypto_pk_write_private_key_to_string(client->client_key,
                                                &client_key_out, &len) != 0) {
        log_warn(LD_BUG, "Internal error: "
                 "crypto_pk_write_private_key_to_string() failed.");
        goto err;
      }
      if (rend_get_service_id(client->client_key, service_id)<0) {
        log_warn(LD_BUG, "Internal error: couldn't encode service ID.");
        /*
         * len is string length, not buffer length, but last byte is NUL
         * anyway.
         */
        memwipe(client_key_out, 0, len);
        tor_free(client_key_out);
        goto err;
      }
      written = tor_snprintf(buf + written, sizeof(buf) - written,
                             "client-key\n%s", client_key_out);
      memwipe(client_key_out, 0, len);
      tor_free(client_key_out);
      if (written < 0) {
        log_warn(LD_BUG, "Could not write client entry.");
        goto err;
      }
    } else {
      strlcpy(service_id, s->service_id, sizeof(service_id));
    }

    if (fputs(buf, cfile) < 0) {
      log_warn(LD_FS, "Could not append client entry to file: %s",
               strerror(errno));
      goto err;
    }

    /* Add line to hostname file. This is not the same encoding as in
     * client_keys. */
    char *encoded_cookie = rend_auth_encode_cookie(client->descriptor_cookie,
                                                   s->auth_type);
    if (!encoded_cookie) {
      log_warn(LD_BUG, "Could not base64-encode descriptor cookie.");
      goto err;
    }
    tor_snprintf(buf, sizeof(buf), "%s.onion %s # client: %s\n",
                 service_id, encoded_cookie, client->client_name);
    memwipe(encoded_cookie, 0, strlen(encoded_cookie));
    tor_free(encoded_cookie);

    if (fputs(buf, hfile)<0) {
      log_warn(LD_FS, "Could not append host entry to file: %s",
               strerror(errno));
      goto err;
    }
  } SMARTLIST_FOREACH_END(client);

  finish_writing_to_file(open_cfile);
  finish_writing_to_file(open_hfile);

  goto done;
 err:
  r = -1;
  if (open_cfile)
    abort_writing_to_file(open_cfile);
  if (open_hfile)
    abort_writing_to_file(open_hfile);
 done:
  if (client_keys_str) {
    memwipe(client_keys_str, 0, strlen(client_keys_str));
    tor_free(client_keys_str);
  }
  strmap_free(parsed_clients, rend_authorized_client_strmap_item_free);

  if (cfname) {
    memwipe(cfname, 0, strlen(cfname));
    tor_free(cfname);
  }

  /* Clear stack buffers that held key-derived material. */
  memwipe(buf, 0, sizeof(buf));
  memwipe(desc_cook_out, 0, sizeof(desc_cook_out));
  memwipe(service_id, 0, sizeof(service_id));

  return r;
}
