rend_config_services(const or_options_t *options, int validate_only)
{
  config_line_t *line;
  rend_service_t *service = NULL;
  rend_service_port_config_t *portcfg;
  smartlist_t *old_service_list = NULL;
  smartlist_t *temp_service_list = NULL;
  int ok = 0;
  int rv = -1;

  /* Use a temporary service list, so that we can check the new services'
   * consistency with each other */
  temp_service_list = smartlist_new();

  for (line = options->RendConfigLines; line; line = line->next) {
    if (!strcasecmp(line->key, "HiddenServiceDir")) {
      /* register the service we just finished parsing
       * this code registers every service except the last one parsed,
       * which is registered below the loop */
      if (rend_service_check_dir_and_add(temp_service_list, options, service,
                                         validate_only) < 0) {
        service = NULL;
        goto free_and_return;
      }
      service = tor_malloc_zero(sizeof(rend_service_t));
      service->directory = tor_strdup(line->value);
      service->ports = smartlist_new();
      service->intro_period_started = time(NULL);
      service->n_intro_points_wanted = NUM_INTRO_POINTS_DEFAULT;
      continue;
    }
    if (!service) {
      log_warn(LD_CONFIG, "%s with no preceding HiddenServiceDir directive",
               line->key);
      goto free_and_return;
    }
    if (!strcasecmp(line->key, "HiddenServicePort")) {
      char *err_msg = NULL;
      portcfg = rend_service_parse_port_config(line->value, " ", &err_msg);
      if (!portcfg) {
        if (err_msg)
          log_warn(LD_CONFIG, "%s", err_msg);
        tor_free(err_msg);
        goto free_and_return;
      }
      tor_assert(!err_msg);
      smartlist_add(service->ports, portcfg);
    } else if (!strcasecmp(line->key, "HiddenServiceAllowUnknownPorts")) {
      service->allow_unknown_ports = (int)tor_parse_long(line->value,
                                                        10, 0, 1, &ok, NULL);
      if (!ok) {
        log_warn(LD_CONFIG,
                 "HiddenServiceAllowUnknownPorts should be 0 or 1, not %s",
                 line->value);
        goto free_and_return;
      }
      log_info(LD_CONFIG,
               "HiddenServiceAllowUnknownPorts=%d for %s",
               (int)service->allow_unknown_ports,
               rend_service_escaped_dir(service));
    } else if (!strcasecmp(line->key,
                           "HiddenServiceDirGroupReadable")) {
        service->dir_group_readable = (int)tor_parse_long(line->value,
                                                        10, 0, 1, &ok, NULL);
        if (!ok) {
            log_warn(LD_CONFIG,
                     "HiddenServiceDirGroupReadable should be 0 or 1, not %s",
                     line->value);
            goto free_and_return;
        }
        log_info(LD_CONFIG,
                 "HiddenServiceDirGroupReadable=%d for %s",
                 service->dir_group_readable,
                 rend_service_escaped_dir(service));
    } else if (!strcasecmp(line->key, "HiddenServiceMaxStreams")) {
      service->max_streams_per_circuit = (int)tor_parse_long(line->value,
                                                    10, 0, 65535, &ok, NULL);
      if (!ok) {
        log_warn(LD_CONFIG,
                 "HiddenServiceMaxStreams should be between 0 and %d, not %s",
                 65535, line->value);
        goto free_and_return;
      }
      log_info(LD_CONFIG,
               "HiddenServiceMaxStreams=%d for %s",
               service->max_streams_per_circuit,
               rend_service_escaped_dir(service));
    } else if (!strcasecmp(line->key, "HiddenServiceMaxStreamsCloseCircuit")) {
      service->max_streams_close_circuit = (int)tor_parse_long(line->value,
                                                        10, 0, 1, &ok, NULL);
      if (!ok) {
        log_warn(LD_CONFIG,
                 "HiddenServiceMaxStreamsCloseCircuit should be 0 or 1, "
                 "not %s",
                 line->value);
        goto free_and_return;
      }
      log_info(LD_CONFIG,
               "HiddenServiceMaxStreamsCloseCircuit=%d for %s",
               (int)service->max_streams_close_circuit,
               rend_service_escaped_dir(service));
    } else if (!strcasecmp(line->key, "HiddenServiceNumIntroductionPoints")) {
      service->n_intro_points_wanted =
        (unsigned int) tor_parse_long(line->value, 10,
                                      0, NUM_INTRO_POINTS_MAX, &ok, NULL);
      if (!ok) {
        log_warn(LD_CONFIG,
                 "HiddenServiceNumIntroductionPoints "
                 "should be between %d and %d, not %s",
                 0, NUM_INTRO_POINTS_MAX, line->value);
        goto free_and_return;
      }
      log_info(LD_CONFIG, "HiddenServiceNumIntroductionPoints=%d for %s",
               service->n_intro_points_wanted,
               rend_service_escaped_dir(service));
    } else if (!strcasecmp(line->key, "HiddenServiceAuthorizeClient")) {
      /* Parse auth type and comma-separated list of client names and add a
       * rend_authorized_client_t for each client to the service's list
       * of authorized clients. */
      smartlist_t *type_names_split, *clients;
      const char *authname;
      int num_clients;
      if (service->auth_type != REND_NO_AUTH) {
        log_warn(LD_CONFIG, "Got multiple HiddenServiceAuthorizeClient "
                 "lines for a single service.");
        goto free_and_return;
      }
      type_names_split = smartlist_new();
      smartlist_split_string(type_names_split, line->value, " ", 0, 2);
      if (smartlist_len(type_names_split) < 1) {
        log_warn(LD_BUG, "HiddenServiceAuthorizeClient has no value. This "
                         "should have been prevented when parsing the "
                         "configuration.");
        goto free_and_return;
      }
      authname = smartlist_get(type_names_split, 0);
      if (!strcasecmp(authname, "basic")) {
        service->auth_type = REND_BASIC_AUTH;
      } else if (!strcasecmp(authname, "stealth")) {
        service->auth_type = REND_STEALTH_AUTH;
      } else {
        log_warn(LD_CONFIG, "HiddenServiceAuthorizeClient contains "
                 "unrecognized auth-type '%s'. Only 'basic' or 'stealth' "
                 "are recognized.",
                 (char *) smartlist_get(type_names_split, 0));
        SMARTLIST_FOREACH(type_names_split, char *, cp, tor_free(cp));
        smartlist_free(type_names_split);
        goto free_and_return;
      }
      service->clients = smartlist_new();
      if (smartlist_len(type_names_split) < 2) {
        log_warn(LD_CONFIG, "HiddenServiceAuthorizeClient contains "
                            "auth-type '%s', but no client names.",
                 service->auth_type == REND_BASIC_AUTH ? "basic" : "stealth");
        SMARTLIST_FOREACH(type_names_split, char *, cp, tor_free(cp));
        smartlist_free(type_names_split);
        continue;
      }
      clients = smartlist_new();
      smartlist_split_string(clients, smartlist_get(type_names_split, 1),
                             ",", SPLIT_SKIP_SPACE, 0);
      SMARTLIST_FOREACH(type_names_split, char *, cp, tor_free(cp));
      smartlist_free(type_names_split);
      /* Remove duplicate client names. */
      num_clients = smartlist_len(clients);
      smartlist_sort_strings(clients);
      smartlist_uniq_strings(clients);
      if (smartlist_len(clients) < num_clients) {
        log_info(LD_CONFIG, "HiddenServiceAuthorizeClient contains %d "
                            "duplicate client name(s); removing.",
                 num_clients - smartlist_len(clients));
        num_clients = smartlist_len(clients);
      }
      SMARTLIST_FOREACH_BEGIN(clients, const char *, client_name)
      {
        rend_authorized_client_t *client;
        if (!rend_valid_client_name(client_name)) {
          log_warn(LD_CONFIG, "HiddenServiceAuthorizeClient contains an "
                              "illegal client name: '%s'. Names must be "
                              "between 1 and %d characters and contain "
                              "only [A-Za-z0-9+_-].",
                   client_name, REND_CLIENTNAME_MAX_LEN);
          SMARTLIST_FOREACH(clients, char *, cp, tor_free(cp));
          smartlist_free(clients);
          goto free_and_return;
        }
        client = tor_malloc_zero(sizeof(rend_authorized_client_t));
        client->client_name = tor_strdup(client_name);
        smartlist_add(service->clients, client);
        log_debug(LD_REND, "Adding client name '%s'", client_name);
      }
      SMARTLIST_FOREACH_END(client_name);
      SMARTLIST_FOREACH(clients, char *, cp, tor_free(cp));
      smartlist_free(clients);
      /* Ensure maximum number of clients. */
      if ((service->auth_type == REND_BASIC_AUTH &&
            smartlist_len(service->clients) > 512) ||
          (service->auth_type == REND_STEALTH_AUTH &&
            smartlist_len(service->clients) > 16)) {
        log_warn(LD_CONFIG, "HiddenServiceAuthorizeClient contains %d "
                            "client authorization entries, but only a "
                            "maximum of %d entries is allowed for "
                            "authorization type '%s'.",
                 smartlist_len(service->clients),
                 service->auth_type == REND_BASIC_AUTH ? 512 : 16,
                 service->auth_type == REND_BASIC_AUTH ? "basic" : "stealth");
        goto free_and_return;
      }
    } else {
      tor_assert(!strcasecmp(line->key, "HiddenServiceVersion"));
      if (strcmp(line->value, "2")) {
        log_warn(LD_CONFIG,
                 "The only supported HiddenServiceVersion is 2.");
        goto free_and_return;
      }
    }
  }
  /* register the final service after we have finished parsing all services
   * this code only registers the last service, other services are registered
   * within the loop. It is ok for this service to be NULL, it is ignored. */
  if (rend_service_check_dir_and_add(temp_service_list, options, service,
                                     validate_only) < 0) {
    service = NULL;
    goto free_and_return;
  }
  service = NULL;

  /* Free the newly added services if validating */
  if (validate_only) {
    rv = 0;
    goto free_and_return;
  }

  /* Otherwise, use the newly added services as the new service list
   * Since we have now replaced the global service list, from this point on we
   * must succeed, or die trying. */
  old_service_list = rend_service_list;
  rend_service_list = temp_service_list;
  temp_service_list = NULL;

  /* If this is a reload and there were hidden services configured before,
   * keep the introduction points that are still needed and close the
   * other ones. */
  if (old_service_list && !validate_only) {
    prune_services_on_reload(old_service_list, rend_service_list);
    /* Every remaining service in the old list have been removed from the
     * configuration so clean them up safely. */
    SMARTLIST_FOREACH(old_service_list, rend_service_t *, s,
                      rend_service_free(s));
    smartlist_free(old_service_list);
  }

  return 0;
 free_and_return:
  rend_service_free(service);
  SMARTLIST_FOREACH(temp_service_list, rend_service_t *, ptr,
                    rend_service_free(ptr));
  smartlist_free(temp_service_list);
  return rv;
}
