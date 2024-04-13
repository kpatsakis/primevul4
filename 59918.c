rend_service_use_direct_connection_node(const or_options_t* options,
                                        const node_t* node)
{
  /* We'll connect directly all reachable addresses, whether preferred or not.
   */
  return (rend_service_allow_non_anonymous_connection(options) &&
          fascist_firewall_allows_node(node, FIREWALL_OR_CONNECTION, 0));
}
