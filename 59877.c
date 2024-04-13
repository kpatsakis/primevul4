rend_consider_services_upload(time_t now)
{
  int i;
  rend_service_t *service;
  const or_options_t *options = get_options();
  int rendpostperiod = options->RendPostPeriod;
  int rendinitialpostdelay = (options->TestingTorNetwork ?
                              MIN_REND_INITIAL_POST_DELAY_TESTING :
                              MIN_REND_INITIAL_POST_DELAY);

  for (i=0; i < smartlist_len(rend_service_list); ++i) {
    service = smartlist_get(rend_service_list, i);
    if (!service->next_upload_time) { /* never been uploaded yet */
      /* The fixed lower bound of rendinitialpostdelay seconds ensures that
       * the descriptor is stable before being published. See comment below. */
      service->next_upload_time =
        now + rendinitialpostdelay + crypto_rand_int(2*rendpostperiod);
      /* Single Onion Services prioritise availability over hiding their
       * startup time, as their IP address is publicly discoverable anyway.
       */
      if (rend_service_reveal_startup_time(options)) {
        service->next_upload_time = now + rendinitialpostdelay;
      }
    }
    /* Does every introduction points have been established? */
    unsigned int intro_points_ready =
      count_established_intro_points(service) >=
        service->n_intro_points_wanted;
    if (intro_points_ready &&
        (service->next_upload_time < now ||
        (service->desc_is_dirty &&
         service->desc_is_dirty < now-rendinitialpostdelay))) {
      /* if it's time, or if the directory servers have a wrong service
       * descriptor and ours has been stable for rendinitialpostdelay seconds,
       * upload a new one of each format. */
      rend_service_update_descriptor(service);
      upload_service_descriptor(service);
    }
  }
}
