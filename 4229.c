ns_input(void)
{
  uint8_t flags = 0;

  LOG_INFO("Received NS from ");
  LOG_INFO_6ADDR(&UIP_IP_BUF->srcipaddr);
  LOG_INFO_(" to ");
  LOG_INFO_6ADDR(&UIP_IP_BUF->destipaddr);
  LOG_INFO_(" with target address ");
  LOG_INFO_6ADDR((uip_ipaddr_t *) (&UIP_ND6_NS_BUF->tgtipaddr));
  LOG_INFO_("\n");
  UIP_STAT(++uip_stat.nd6.recv);

#if UIP_CONF_IPV6_CHECKS
  if((UIP_IP_BUF->ttl != UIP_ND6_HOP_LIMIT) ||
     (uip_is_addr_mcast(&UIP_ND6_NS_BUF->tgtipaddr)) ||
     (UIP_ICMP_BUF->icode != 0)) {
    LOG_ERR("NS received is bad\n");
    goto discard;
  }
#endif /* UIP_CONF_IPV6_CHECKS */

  /* Options processing */
  nd6_opt_llao = NULL;
  nd6_opt_offset = UIP_ND6_NS_LEN;
  while(uip_l3_icmp_hdr_len + nd6_opt_offset + UIP_ND6_OPT_HDR_LEN < uip_len) {
#if UIP_CONF_IPV6_CHECKS
    if(ND6_OPT_HDR_BUF(nd6_opt_offset)->len == 0) {
      LOG_ERR("NS received is bad\n");
      goto discard;
    }
#endif /* UIP_CONF_IPV6_CHECKS */
    switch (ND6_OPT_HDR_BUF(nd6_opt_offset)->type) {
    case UIP_ND6_OPT_SLLAO:
      if(uip_l3_icmp_hdr_len + nd6_opt_offset +
         UIP_ND6_OPT_DATA_OFFSET + UIP_LLADDR_LEN > uip_len) {
        LOG_ERR("Insufficient data for NS SLLAO option\n");
        goto discard;
      }
      nd6_opt_llao = &uip_buf[uip_l3_icmp_hdr_len + nd6_opt_offset];
#if UIP_CONF_IPV6_CHECKS
      /* There must be NO option in a DAD NS */
      if(uip_is_addr_unspecified(&UIP_IP_BUF->srcipaddr)) {
        LOG_ERR("NS received is bad\n");
        goto discard;
      } else {
#endif /*UIP_CONF_IPV6_CHECKS */
        uip_lladdr_t lladdr_aligned;
        extract_lladdr_from_llao_aligned(&lladdr_aligned);
        nbr = uip_ds6_nbr_lookup(&UIP_IP_BUF->srcipaddr);
        if(nbr == NULL) {
          uip_ds6_nbr_add(&UIP_IP_BUF->srcipaddr, &lladdr_aligned,
			  0, NBR_STALE, NBR_TABLE_REASON_IPV6_ND, NULL);
        } else {
          const uip_lladdr_t *lladdr = uip_ds6_nbr_get_ll(nbr);
          if(lladdr == NULL) {
            goto discard;
          }
          if(memcmp(&nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET],
              lladdr, UIP_LLADDR_LEN) != 0) {
            if(uip_ds6_nbr_update_ll(&nbr,
                                     (const uip_lladdr_t *)&lladdr_aligned)
               < 0) {
              /* failed to update the lladdr */
              goto discard;
            }
            nbr->state = NBR_STALE;
          } else {
            if(nbr->state == NBR_INCOMPLETE) {
              nbr->state = NBR_STALE;
            }
          }
        }
#if UIP_CONF_IPV6_CHECKS
      }
#endif /*UIP_CONF_IPV6_CHECKS */
      break;
    default:
      LOG_WARN("ND option not supported in NS");
      break;
    }
    nd6_opt_offset += (ND6_OPT_HDR_BUF(nd6_opt_offset)->len << 3);
  }

  addr = uip_ds6_addr_lookup(&UIP_ND6_NS_BUF->tgtipaddr);
  if(addr != NULL) {
    if(uip_is_addr_unspecified(&UIP_IP_BUF->srcipaddr)) {
      /* DAD CASE */
#if UIP_ND6_DEF_MAXDADNS > 0
#if UIP_CONF_IPV6_CHECKS
      if(!uip_is_addr_solicited_node(&UIP_IP_BUF->destipaddr)) {
        LOG_ERR("NS received is bad\n");
        goto discard;
      }
#endif /* UIP_CONF_IPV6_CHECKS */
      if(addr->state != ADDR_TENTATIVE) {
        uip_create_linklocal_allnodes_mcast(&UIP_IP_BUF->destipaddr);
        uip_ds6_select_src(&UIP_IP_BUF->srcipaddr, &UIP_IP_BUF->destipaddr);
        flags = UIP_ND6_NA_FLAG_OVERRIDE;
        goto create_na;
      } else {
          /** \todo if I sent a NS before him, I win */
        uip_ds6_dad_failed(addr);
        goto discard;
      }
#else /* UIP_ND6_DEF_MAXDADNS > 0 */
      goto discard;  /* DAD CASE */
#endif /* UIP_ND6_DEF_MAXDADNS > 0 */
    }
#if UIP_CONF_IPV6_CHECKS
    if(uip_ds6_is_my_addr(&UIP_IP_BUF->srcipaddr)) {
        /**
         * \NOTE do we do something here? we both are using the same address.
         * If we are doing dad, we could cancel it, though we should receive a
         * NA in response of DAD NS we sent, hence DAD will fail anyway. If we
         * were not doing DAD, it means there is a duplicate in the network!
         */
      LOG_ERR("NS received is bad\n");
      goto discard;
    }
#endif /*UIP_CONF_IPV6_CHECKS */

    /* Address resolution case */
    if(uip_is_addr_solicited_node(&UIP_IP_BUF->destipaddr)) {
      uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, &UIP_IP_BUF->srcipaddr);
      uip_ipaddr_copy(&UIP_IP_BUF->srcipaddr, &UIP_ND6_NS_BUF->tgtipaddr);
      flags = UIP_ND6_NA_FLAG_SOLICITED | UIP_ND6_NA_FLAG_OVERRIDE;
      goto create_na;
    }

    /* NUD CASE */
    if(uip_ds6_addr_lookup(&UIP_IP_BUF->destipaddr) == addr) {
      uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, &UIP_IP_BUF->srcipaddr);
      uip_ipaddr_copy(&UIP_IP_BUF->srcipaddr, &UIP_ND6_NS_BUF->tgtipaddr);
      flags = UIP_ND6_NA_FLAG_SOLICITED | UIP_ND6_NA_FLAG_OVERRIDE;
      goto create_na;
    } else {
#if UIP_CONF_IPV6_CHECKS
      LOG_ERR("NS received is bad\n");
      goto discard;
#endif /* UIP_CONF_IPV6_CHECKS */
    }
  } else {
    goto discard;
  }


create_na:
    /* If the node is a router it should set R flag in NAs */
#if UIP_CONF_ROUTER
    flags = flags | UIP_ND6_NA_FLAG_ROUTER;
#endif
  uipbuf_clear();
  UIP_IP_BUF->vtc = 0x60;
  UIP_IP_BUF->tcflow = 0;
  UIP_IP_BUF->flow = 0;
  uipbuf_set_len_field(UIP_IP_BUF, UIP_ICMPH_LEN + UIP_ND6_NA_LEN + UIP_ND6_OPT_LLAO_LEN);
  UIP_IP_BUF->proto = UIP_PROTO_ICMP6;
  UIP_IP_BUF->ttl = UIP_ND6_HOP_LIMIT;

  UIP_ICMP_BUF->type = ICMP6_NA;
  UIP_ICMP_BUF->icode = 0;

  UIP_ND6_NA_BUF->flagsreserved = flags;
  memcpy(&UIP_ND6_NA_BUF->tgtipaddr, &addr->ipaddr, sizeof(uip_ipaddr_t));

  create_llao(&uip_buf[uip_l3_icmp_hdr_len + UIP_ND6_NA_LEN],
              UIP_ND6_OPT_TLLAO);

  UIP_ICMP_BUF->icmpchksum = 0;
  UIP_ICMP_BUF->icmpchksum = ~uip_icmp6chksum();

  uipbuf_set_len(UIP_IPH_LEN + UIP_ICMPH_LEN + UIP_ND6_NA_LEN + UIP_ND6_OPT_LLAO_LEN);

  UIP_STAT(++uip_stat.nd6.sent);
  LOG_INFO("Sending NA to ");
  LOG_INFO_6ADDR(&UIP_IP_BUF->destipaddr);
  LOG_INFO_(" from ");
  LOG_INFO_6ADDR(&UIP_IP_BUF->srcipaddr);
  LOG_INFO_(" with target address ");
  LOG_INFO_6ADDR(&UIP_ND6_NA_BUF->tgtipaddr);
  LOG_INFO_("\n");
  return;

discard:
  uipbuf_clear();
  return;
}