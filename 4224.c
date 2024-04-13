uip_nd6_ra_output(uip_ipaddr_t * dest)
{

  UIP_IP_BUF->vtc = 0x60;
  UIP_IP_BUF->tcflow = 0;
  UIP_IP_BUF->flow = 0;
  UIP_IP_BUF->proto = UIP_PROTO_ICMP6;
  UIP_IP_BUF->ttl = UIP_ND6_HOP_LIMIT;

  if(dest == NULL) {
    uip_create_linklocal_allnodes_mcast(&UIP_IP_BUF->destipaddr);
  } else {
    /* For sollicited RA */
    uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, dest);
  }
  uip_ds6_select_src(&UIP_IP_BUF->srcipaddr, &UIP_IP_BUF->destipaddr);

  UIP_ICMP_BUF->type = ICMP6_RA;
  UIP_ICMP_BUF->icode = 0;

  UIP_ND6_RA_BUF->cur_ttl = uip_ds6_if.cur_hop_limit;

  UIP_ND6_RA_BUF->flags_reserved =
    (UIP_ND6_M_FLAG << 7) | (UIP_ND6_O_FLAG << 6);

  UIP_ND6_RA_BUF->router_lifetime = uip_htons(UIP_ND6_ROUTER_LIFETIME);
  //UIP_ND6_RA_BUF->reachable_time = uip_htonl(uip_ds6_if.reachable_time);
  //UIP_ND6_RA_BUF->retrans_timer = uip_htonl(uip_ds6_if.retrans_timer);
  UIP_ND6_RA_BUF->reachable_time = 0;
  UIP_ND6_RA_BUF->retrans_timer = 0;

  uip_len = UIP_IPH_LEN + UIP_ICMPH_LEN + UIP_ND6_RA_LEN;
  nd6_opt_offset = UIP_ND6_RA_LEN;


  /* Prefix list */
  for(prefix = uip_ds6_prefix_list;
      prefix < uip_ds6_prefix_list + UIP_DS6_PREFIX_NB; prefix++) {
    if((prefix->isused) && (prefix->advertise)) {
      ND6_OPT_PREFIX_BUF(nd6_opt_offset)->type = UIP_ND6_OPT_PREFIX_INFO;
      ND6_OPT_PREFIX_BUF(nd6_opt_offset)->len = UIP_ND6_OPT_PREFIX_INFO_LEN / 8;
      ND6_OPT_PREFIX_BUF(nd6_opt_offset)->preflen = prefix->length;
      ND6_OPT_PREFIX_BUF(nd6_opt_offset)->flagsreserved1 = prefix->l_a_reserved;
      ND6_OPT_PREFIX_BUF(nd6_opt_offset)->validlt = uip_htonl(prefix->vlifetime);
      ND6_OPT_PREFIX_BUF(nd6_opt_offset)->preferredlt = uip_htonl(prefix->plifetime);
      ND6_OPT_PREFIX_BUF(nd6_opt_offset)->reserved2 = 0;
      uip_ipaddr_copy(&(ND6_OPT_PREFIX_BUF(nd6_opt_offset)->prefix), &(prefix->ipaddr));
      nd6_opt_offset += UIP_ND6_OPT_PREFIX_INFO_LEN;
      uip_len += UIP_ND6_OPT_PREFIX_INFO_LEN;
    }
  }

  /* Source link-layer option */
  create_llao((uint8_t *)ND6_OPT_HDR_BUF(nd6_opt_offset), UIP_ND6_OPT_SLLAO);

  uip_len += UIP_ND6_OPT_LLAO_LEN;
  nd6_opt_offset += UIP_ND6_OPT_LLAO_LEN;

  /* MTU */
  ND6_OPT_MTU_BUF(nd6_opt_offset)->type = UIP_ND6_OPT_MTU;
  ND6_OPT_MTU_BUF(nd6_opt_offset)->len = UIP_ND6_OPT_MTU_LEN >> 3;
  ND6_OPT_MTU_BUF(nd6_opt_offset)->reserved = 0;
  //ND6_OPT_MTU_BUF(nd6_opt_offset)->mtu = uip_htonl(uip_ds6_if.link_mtu);
  ND6_OPT_MTU_BUF(nd6_opt_offset)->mtu = uip_htonl(1500);

  uip_len += UIP_ND6_OPT_MTU_LEN;
  nd6_opt_offset += UIP_ND6_OPT_MTU_LEN;

#if UIP_ND6_RA_RDNSS
  if(uip_nameserver_count() > 0) {
    uint8_t i = 0;
    uip_ipaddr_t *ip = &ND6_OPT_RDNSS_BUF(nd6_opt_offset)->ip;
    uip_ipaddr_t *dns = NULL;
    ND6_OPT_RDNSS_BUF(nd6_opt_offset)->type = UIP_ND6_OPT_RDNSS;
    ND6_OPT_RDNSS_BUF(nd6_opt_offset)->reserved = 0;
    ND6_OPT_RDNSS_BUF(nd6_opt_offset)->lifetime = uip_nameserver_next_expiration();
    if(ND6_OPT_RDNSS_BUF(nd6_opt_offset)->lifetime != UIP_NAMESERVER_INFINITE_LIFETIME) {
      ND6_OPT_RDNSS_BUF(nd6_opt_offset)->lifetime -= clock_seconds();
    }
    while((dns = uip_nameserver_get(i)) != NULL) {
      uip_ipaddr_copy(ip++, dns);
      i++;
    }
    ND6_OPT_RDNSS_BUF(nd6_opt_offset)->len = UIP_ND6_OPT_RDNSS_LEN + (i << 1);
    LOG_INFO("%d nameservers reported\n", i);
    uip_len += ND6_OPT_RDNSS_BUF(nd6_opt_offset)->len << 3;
    nd6_opt_offset += ND6_OPT_RDNSS_BUF(nd6_opt_offset)->len << 3;
  }
#endif /* UIP_ND6_RA_RDNSS */

  uipbuf_set_len_field(UIP_IP_BUF, uip_len - UIP_IPH_LEN);

  /*ICMP checksum */
  UIP_ICMP_BUF->icmpchksum = 0;
  UIP_ICMP_BUF->icmpchksum = ~uip_icmp6chksum();

  UIP_STAT(++uip_stat.nd6.sent);
  LOG_INFO("Sending RA to ");
  LOG_INFO_6ADDR(&UIP_IP_BUF->destipaddr);
  LOG_INFO_(" from ");
  LOG_INFO_6ADDR(&UIP_IP_BUF->srcipaddr);
  LOG_INFO_("\n");
  return;
}