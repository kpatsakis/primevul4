na_input(void)
{
  uint8_t is_llchange;
  uint8_t is_router;
  uint8_t is_solicited;
  uint8_t is_override;
  uip_lladdr_t lladdr_aligned;

  LOG_INFO("Received NA from ");
  LOG_INFO_6ADDR(&UIP_IP_BUF->srcipaddr);
  LOG_INFO_(" to ");
  LOG_INFO_6ADDR(&UIP_IP_BUF->destipaddr);
  LOG_INFO_(" with target address ");
  LOG_INFO_6ADDR((uip_ipaddr_t *) (&UIP_ND6_NA_BUF->tgtipaddr));
  LOG_INFO_("\n");
  UIP_STAT(++uip_stat.nd6.recv);

  /*
   * booleans. the three last one are not 0 or 1 but 0 or 0x80, 0x40, 0x20
   * but it works. Be careful though, do not use tests such as is_router == 1
   */
  is_llchange = 0;
  is_router = ((UIP_ND6_NA_BUF->flagsreserved & UIP_ND6_NA_FLAG_ROUTER));
  is_solicited =
    ((UIP_ND6_NA_BUF->flagsreserved & UIP_ND6_NA_FLAG_SOLICITED));
  is_override =
    ((UIP_ND6_NA_BUF->flagsreserved & UIP_ND6_NA_FLAG_OVERRIDE));

#if UIP_CONF_IPV6_CHECKS
  if((UIP_IP_BUF->ttl != UIP_ND6_HOP_LIMIT) ||
     (UIP_ICMP_BUF->icode != 0) ||
     (uip_is_addr_mcast(&UIP_ND6_NA_BUF->tgtipaddr)) ||
     (is_solicited && uip_is_addr_mcast(&UIP_IP_BUF->destipaddr))) {
    LOG_ERR("NA received is bad\n");
    goto discard;
  }
#endif /*UIP_CONF_IPV6_CHECKS */

  /* Options processing: we handle TLLAO, and must ignore others */
  nd6_opt_offset = UIP_ND6_NA_LEN;
  nd6_opt_llao = NULL;
  while(uip_l3_icmp_hdr_len + nd6_opt_offset < uip_len) {
#if UIP_CONF_IPV6_CHECKS
    if(ND6_OPT_HDR_BUF(nd6_opt_offset)->len == 0) {
      LOG_ERR("NA received is bad\n");
      goto discard;
    }
#endif /*UIP_CONF_IPV6_CHECKS */
    switch (ND6_OPT_HDR_BUF(nd6_opt_offset)->type) {
    case UIP_ND6_OPT_TLLAO:
      nd6_opt_llao = (uint8_t *)ND6_OPT_HDR_BUF(nd6_opt_offset);
      break;
    default:
      LOG_WARN("ND option not supported in NA\n");
      break;
    }
    nd6_opt_offset += (ND6_OPT_HDR_BUF(nd6_opt_offset)->len << 3);
  }
  addr = uip_ds6_addr_lookup(&UIP_ND6_NA_BUF->tgtipaddr);
  /* Message processing, including TLLAO if any */
  if(addr != NULL) {
#if UIP_ND6_DEF_MAXDADNS > 0
    if(addr->state == ADDR_TENTATIVE) {
      uip_ds6_dad_failed(addr);
    }
#endif /*UIP_ND6_DEF_MAXDADNS > 0 */
    LOG_ERR("NA received is bad\n");
    goto discard;
  } else {
    const uip_lladdr_t *lladdr;
    nbr = uip_ds6_nbr_lookup(&UIP_ND6_NA_BUF->tgtipaddr);
    if(nbr == NULL) {
      goto discard;
    }
    lladdr = uip_ds6_nbr_get_ll(nbr);
    if(lladdr == NULL) {
      goto discard;
    }
    if(nd6_opt_llao != NULL) {
      is_llchange =
        memcmp(&nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET], lladdr,
               UIP_LLADDR_LEN) == 0 ? 0 : 1;
    }
    if(nbr->state == NBR_INCOMPLETE) {
      if(nd6_opt_llao == NULL || !extract_lladdr_from_llao_aligned(&lladdr_aligned)) {
        goto discard;
      }
      if(uip_ds6_nbr_update_ll(&nbr,
                               (const uip_lladdr_t *)&lladdr_aligned) < 0) {
        /* failed to update the lladdr */
        goto discard;
      }

      /* Note: No need to refresh the state of the nbr here.
       * It has already been refreshed upon receiving the unicast IPv6 ND packet.
       * See: uip_ds6_nbr_refresh_reachable_state()
       */
      if(!is_solicited) {
        nbr->state = NBR_STALE;
      }
      nbr->isrouter = is_router;
    } else { /* NBR is not INCOMPLETE */
      if(!is_override && is_llchange) {
        if(nbr->state == NBR_REACHABLE) {
          nbr->state = NBR_STALE;
        }
        goto discard;
      } else {
        /**
         *  If this is an cache override, or same lladdr, or no llao -
         *  do updates of nbr states.
         */
        if(is_override || !is_llchange || nd6_opt_llao == NULL) {
          if(nd6_opt_llao != NULL && is_llchange) {
            if(!extract_lladdr_from_llao_aligned(&lladdr_aligned) ||
               uip_ds6_nbr_update_ll(&nbr,
                                     (const uip_lladdr_t *)&lladdr_aligned)
               < 0) {
              /* failed to update the lladdr */
              goto discard;
            }
          }
          /* Note: No need to refresh the state of the nbr here.
           * It has already been refreshed upon receiving the unicast IPv6 ND packet.
           * See: uip_ds6_nbr_refresh_reachable_state()
           */
        }
      }
      if(nbr->isrouter && !is_router) {
        defrt = uip_ds6_defrt_lookup(&UIP_IP_BUF->srcipaddr);
        if(defrt != NULL) {
          uip_ds6_defrt_rm(defrt);
        }
      }
      nbr->isrouter = is_router;
    }
  }
#if UIP_CONF_IPV6_QUEUE_PKT
  /* The nbr is now reachable, check if we had buffered a pkt for it */
  /*if(nbr->queue_buf_len != 0) {
    uip_len = nbr->queue_buf_len;
    memcpy(UIP_IP_BUF, nbr->queue_buf, uip_len);
    nbr->queue_buf_len = 0;
    return;
    }*/
  if(uip_packetqueue_buflen(&nbr->packethandle) != 0) {
    uip_len = uip_packetqueue_buflen(&nbr->packethandle);
    memcpy(UIP_IP_BUF, uip_packetqueue_buf(&nbr->packethandle), uip_len);
    uip_packetqueue_free(&nbr->packethandle);
    return;
  }

#endif /*UIP_CONF_IPV6_QUEUE_PKT */

discard:
  uipbuf_clear();
  return;
}