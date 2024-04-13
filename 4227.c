uip_nd6_init()
{
#if UIP_ND6_SEND_NA
  /* Only handle NSs if we are prepared to send out NAs */
  uip_icmp6_register_input_handler(&ns_input_handler);
#endif

#if UIP_ND6_SEND_NS
  /*
   * Only handle NAs if we are prepared to send out NSs. */
  uip_icmp6_register_input_handler(&na_input_handler);
#endif

#if UIP_CONF_ROUTER && UIP_ND6_SEND_RA
  /* Only accept RS if we are a router and happy to send out RAs */
  uip_icmp6_register_input_handler(&rs_input_handler);
#endif

#if !UIP_CONF_ROUTER
  /* Only process RAs if we are not a router */
  uip_icmp6_register_input_handler(&ra_input_handler);
#endif
}