rend_service_free(rend_service_t *service)
{
  if (!service)
    return;

  tor_free(service->directory);
  if (service->ports) {
    SMARTLIST_FOREACH(service->ports, rend_service_port_config_t*, p,
                      rend_service_port_config_free(p));
    smartlist_free(service->ports);
  }
  if (service->private_key)
    crypto_pk_free(service->private_key);
  if (service->intro_nodes) {
    SMARTLIST_FOREACH(service->intro_nodes, rend_intro_point_t *, intro,
      rend_intro_point_free(intro););
    smartlist_free(service->intro_nodes);
  }
  if (service->expiring_nodes) {
    SMARTLIST_FOREACH(service->expiring_nodes, rend_intro_point_t *, intro,
                      rend_intro_point_free(intro););
    smartlist_free(service->expiring_nodes);
  }

  rend_service_descriptor_free(service->desc);
  if (service->clients) {
    SMARTLIST_FOREACH(service->clients, rend_authorized_client_t *, c,
      rend_authorized_client_free(c););
    smartlist_free(service->clients);
  }
  if (service->accepted_intro_dh_parts) {
    replaycache_free(service->accepted_intro_dh_parts);
  }
  tor_free(service);
}
