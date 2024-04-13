rend_check_authorization(rend_service_t *service,
                         const char *descriptor_cookie,
                         size_t cookie_len)
{
  rend_authorized_client_t *auth_client = NULL;
  tor_assert(service);
  tor_assert(descriptor_cookie);
  if (!service->clients) {
    log_warn(LD_BUG, "Can't check authorization for a service that has no "
                     "authorized clients configured.");
    return 0;
  }

  if (cookie_len != REND_DESC_COOKIE_LEN) {
    log_info(LD_REND, "Descriptor cookie is %lu bytes, but we expected "
                      "%lu bytes. Dropping cell.",
             (unsigned long)cookie_len, (unsigned long)REND_DESC_COOKIE_LEN);
    return 0;
  }

  /* Look up client authorization by descriptor cookie. */
  SMARTLIST_FOREACH(service->clients, rend_authorized_client_t *, client, {
    if (tor_memeq(client->descriptor_cookie, descriptor_cookie,
                REND_DESC_COOKIE_LEN)) {
      auth_client = client;
      break;
    }
  });
  if (!auth_client) {
    char descriptor_cookie_base64[3*REND_DESC_COOKIE_LEN_BASE64];
    base64_encode(descriptor_cookie_base64, sizeof(descriptor_cookie_base64),
                  descriptor_cookie, REND_DESC_COOKIE_LEN, 0);
    log_info(LD_REND, "No authorization found for descriptor cookie '%s'! "
                      "Dropping cell!",
             descriptor_cookie_base64);
    return 0;
  }

  /* Allow the request. */
  log_info(LD_REND, "Client %s authorized for service %s.",
           auth_client->client_name, service->service_id);
  return 1;
}
