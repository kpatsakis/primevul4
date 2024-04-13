rend_service_set_connection_addr_port(edge_connection_t *conn,
                                      origin_circuit_t *circ)
{
  rend_service_t *service;
  char serviceid[REND_SERVICE_ID_LEN_BASE32+1];
  smartlist_t *matching_ports;
  rend_service_port_config_t *chosen_port;
  unsigned int warn_once = 0;
  const char *rend_pk_digest;

  tor_assert(circ->base_.purpose == CIRCUIT_PURPOSE_S_REND_JOINED);
  tor_assert(circ->rend_data);
  log_debug(LD_REND,"beginning to hunt for addr/port");
  /* XXX: This is version 2 specific (only one supported). */
  rend_pk_digest = (char *) rend_data_get_pk_digest(circ->rend_data, NULL);
  base32_encode(serviceid, REND_SERVICE_ID_LEN_BASE32+1,
                rend_pk_digest, REND_SERVICE_ID_LEN);
  service = rend_service_get_by_pk_digest(rend_pk_digest);
  if (!service) {
    log_warn(LD_REND, "Couldn't find any service associated with pk %s on "
             "rendezvous circuit %u; closing.",
             serviceid, (unsigned)circ->base_.n_circ_id);
    return -2;
  }
  if (service->max_streams_per_circuit > 0) {
    /* Enforce the streams-per-circuit limit, and refuse to provide a
     * mapping if this circuit will exceed the limit. */
#define MAX_STREAM_WARN_INTERVAL 600
    static struct ratelim_t stream_ratelim =
        RATELIM_INIT(MAX_STREAM_WARN_INTERVAL);
    if (circ->rend_data->nr_streams >= service->max_streams_per_circuit) {
      log_fn_ratelim(&stream_ratelim, LOG_WARN, LD_REND,
                     "Maximum streams per circuit limit reached on rendezvous "
                     "circuit %u; %s.  Circuit has %d out of %d streams.",
                     (unsigned)circ->base_.n_circ_id,
                     service->max_streams_close_circuit ?
                       "closing circuit" :
                       "ignoring open stream request",
                     circ->rend_data->nr_streams,
                     service->max_streams_per_circuit);
      return service->max_streams_close_circuit ? -2 : -1;
    }
  }
  matching_ports = smartlist_new();
  SMARTLIST_FOREACH(service->ports, rend_service_port_config_t *, p,
  {
    if (conn->base_.port != p->virtual_port) {
      continue;
    }
    if (!(p->is_unix_addr)) {
      smartlist_add(matching_ports, p);
    } else {
      if (add_unix_port(matching_ports, p)) {
        if (!warn_once) {
         /* Unix port not supported so warn only once. */
          log_warn(LD_REND,
              "Saw AF_UNIX virtual port mapping for port %d on service "
              "%s, which is unsupported on this platform. Ignoring it.",
              conn->base_.port, serviceid);
        }
        warn_once++;
      }
    }
  });
  chosen_port = smartlist_choose(matching_ports);
  smartlist_free(matching_ports);
  if (chosen_port) {
    if (!(chosen_port->is_unix_addr)) {
      /* Get a non-AF_UNIX connection ready for connection_exit_connect() */
      tor_addr_copy(&conn->base_.addr, &chosen_port->real_addr);
      conn->base_.port = chosen_port->real_port;
    } else {
      if (set_unix_port(conn, chosen_port)) {
        /* Simply impossible to end up here else we were able to add a Unix
         * port without AF_UNIX support... ? */
        tor_assert(0);
      }
    }
    return 0;
  }

  log_info(LD_REND,
           "No virtual port mapping exists for port %d on service %s",
           conn->base_.port, serviceid);

  if (service->allow_unknown_ports)
    return -1;
  else
    return -2;
}
