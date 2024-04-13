uip_nd6_ns_output(uip_ipaddr_t * src, uip_ipaddr_t * dest, uip_ipaddr_t * tgt)
{
  uipbuf_clear();
  UIP_IP_BUF->vtc = 0x60;
  UIP_IP_BUF->tcflow = 0;
  UIP_IP_BUF->flow = 0;
  UIP_IP_BUF->proto = UIP_PROTO_ICMP6;
  UIP_IP_BUF->ttl = UIP_ND6_HOP_LIMIT;

  if(dest == NULL) {
    uip_create_solicited_node(tgt, &UIP_IP_BUF->destipaddr);
  } else {
    uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, dest);
  }
  UIP_ICMP_BUF->type = ICMP6_NS;
  UIP_ICMP_BUF->icode = 0;
  UIP_ND6_NS_BUF->reserved = 0;
  uip_ipaddr_copy((uip_ipaddr_t *) &UIP_ND6_NS_BUF->tgtipaddr, tgt);
  /*
   * check if we add a SLLAO option: for DAD, MUST NOT, for NUD, MAY
   * (here yes), for Address resolution , MUST
   */
  if(!(uip_ds6_is_my_addr(tgt))) {
    if(src != NULL) {
      uip_ipaddr_copy(&UIP_IP_BUF->srcipaddr, src);
    } else {
      uip_ds6_select_src(&UIP_IP_BUF->srcipaddr, &UIP_IP_BUF->destipaddr);
    }
    if (uip_is_addr_unspecified(&UIP_IP_BUF->srcipaddr)) {
      LOG_ERR("Dropping NS due to no suitable source address\n");
      uipbuf_clear();
      return;
    }
    uipbuf_set_len_field(UIP_IP_BUF, UIP_ICMPH_LEN + UIP_ND6_NS_LEN + UIP_ND6_OPT_LLAO_LEN);

    create_llao(&uip_buf[uip_l3_icmp_hdr_len + UIP_ND6_NS_LEN],
                UIP_ND6_OPT_SLLAO);

    uip_len =
      UIP_IPH_LEN + UIP_ICMPH_LEN + UIP_ND6_NS_LEN + UIP_ND6_OPT_LLAO_LEN;
  } else {
    uip_create_unspecified(&UIP_IP_BUF->srcipaddr);
    UIP_IP_BUF->len[1] = UIP_ICMPH_LEN + UIP_ND6_NS_LEN;
    uip_len = UIP_IPH_LEN + UIP_ICMPH_LEN + UIP_ND6_NS_LEN;
  }

  UIP_ICMP_BUF->icmpchksum = 0;
  UIP_ICMP_BUF->icmpchksum = ~uip_icmp6chksum();

  UIP_STAT(++uip_stat.nd6.sent);
  LOG_INFO("Sending NS to ");
  LOG_INFO_6ADDR(&UIP_IP_BUF->destipaddr);
  LOG_INFO_(" from ");
  LOG_INFO_6ADDR(&UIP_IP_BUF->srcipaddr);
  LOG_INFO_(" with target address ");
  LOG_INFO_6ADDR(tgt);
  LOG_INFO_("\n");
  return;
}