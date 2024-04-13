directory_post_to_hs_dir(rend_service_descriptor_t *renddesc,
                         smartlist_t *descs, smartlist_t *hs_dirs,
                         const char *service_id, int seconds_valid)
{
  int i, j, failed_upload = 0;
  smartlist_t *responsible_dirs = smartlist_new();
  smartlist_t *successful_uploads = smartlist_new();
  routerstatus_t *hs_dir;
  for (i = 0; i < smartlist_len(descs); i++) {
    rend_encoded_v2_service_descriptor_t *desc = smartlist_get(descs, i);
    /** If any HSDirs are specified, they should be used instead of
     *  the responsible directories */
    if (hs_dirs && smartlist_len(hs_dirs) > 0) {
      smartlist_add_all(responsible_dirs, hs_dirs);
    } else {
      /* Determine responsible dirs. */
      if (hid_serv_get_responsible_directories(responsible_dirs,
                                               desc->desc_id) < 0) {
        log_warn(LD_REND, "Could not determine the responsible hidden service "
                          "directories to post descriptors to.");
        control_event_hs_descriptor_upload(service_id,
                                           "UNKNOWN",
                                           "UNKNOWN");
        goto done;
      }
    }
    for (j = 0; j < smartlist_len(responsible_dirs); j++) {
      char desc_id_base32[REND_DESC_ID_V2_LEN_BASE32 + 1];
      char *hs_dir_ip;
      const node_t *node;
      rend_data_t *rend_data;
      hs_dir = smartlist_get(responsible_dirs, j);
      if (smartlist_contains_digest(renddesc->successful_uploads,
                                hs_dir->identity_digest))
        /* Don't upload descriptor if we succeeded in doing so last time. */
        continue;
      node = node_get_by_id(hs_dir->identity_digest);
      if (!node || !node_has_descriptor(node)) {
        log_info(LD_REND, "Not launching upload for for v2 descriptor to "
                          "hidden service directory %s; we don't have its "
                          "router descriptor. Queuing for later upload.",
                 safe_str_client(routerstatus_describe(hs_dir)));
        failed_upload = -1;
        continue;
      }
      /* Send publish request. */

      /* We need the service ID to identify which service did the upload
       * request. Lookup is made in rend_service_desc_has_uploaded(). */
      rend_data = rend_data_client_create(service_id, desc->desc_id, NULL,
                                          REND_NO_AUTH);
      directory_initiate_command_routerstatus_rend(hs_dir,
                                              DIR_PURPOSE_UPLOAD_RENDDESC_V2,
                                                   ROUTER_PURPOSE_GENERAL,
                                                   DIRIND_ANONYMOUS, NULL,
                                                   desc->desc_str,
                                                   strlen(desc->desc_str),
                                                   0, rend_data, NULL);
      rend_data_free(rend_data);
      base32_encode(desc_id_base32, sizeof(desc_id_base32),
                    desc->desc_id, DIGEST_LEN);
      hs_dir_ip = tor_dup_ip(hs_dir->addr);
      log_info(LD_REND, "Launching upload for v2 descriptor for "
                        "service '%s' with descriptor ID '%s' with validity "
                        "of %d seconds to hidden service directory '%s' on "
                        "%s:%d.",
               safe_str_client(service_id),
               safe_str_client(desc_id_base32),
               seconds_valid,
               hs_dir->nickname,
               hs_dir_ip,
               hs_dir->or_port);
      control_event_hs_descriptor_upload(service_id,
                                         hs_dir->identity_digest,
                                         desc_id_base32);
      tor_free(hs_dir_ip);
      /* Remember successful upload to this router for next time. */
      if (!smartlist_contains_digest(successful_uploads,
                                     hs_dir->identity_digest))
        smartlist_add(successful_uploads, hs_dir->identity_digest);
    }
    smartlist_clear(responsible_dirs);
  }
  if (!failed_upload) {
    if (renddesc->successful_uploads) {
      SMARTLIST_FOREACH(renddesc->successful_uploads, char *, c, tor_free(c););
      smartlist_free(renddesc->successful_uploads);
      renddesc->successful_uploads = NULL;
    }
    renddesc->all_uploads_performed = 1;
  } else {
    /* Remember which routers worked this time, so that we don't upload the
     * descriptor to them again. */
    if (!renddesc->successful_uploads)
      renddesc->successful_uploads = smartlist_new();
    SMARTLIST_FOREACH(successful_uploads, const char *, c, {
      if (!smartlist_contains_digest(renddesc->successful_uploads, c)) {
        char *hsdir_id = tor_memdup(c, DIGEST_LEN);
        smartlist_add(renddesc->successful_uploads, hsdir_id);
      }
    });
  }
 done:
  smartlist_free(responsible_dirs);
  smartlist_free(successful_uploads);
}
