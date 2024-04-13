lldp_network_addr_print(netdissect_options *ndo, const u_char *tptr, u_int len)
{
    uint8_t af;
    static char buf[BUFSIZE];
    const char * (*pfunc)(netdissect_options *, const u_char *);

    if (len < 1)
      return NULL;
    len--;
    af = *tptr;
    switch (af) {
    case AFNUM_INET:
        if (len < 4)
          return NULL;
        /* This cannot be assigned to ipaddr_string(), which is a macro. */
        pfunc = getname;
        break;
    case AFNUM_INET6:
        if (len < 16)
          return NULL;
        /* This cannot be assigned to ip6addr_string(), which is a macro. */
        pfunc = getname6;
        break;
    case AFNUM_802:
        if (len < 6)
          return NULL;
        pfunc = etheraddr_string;
        break;
    default:
        pfunc = NULL;
        break;
    }

    if (!pfunc) {
        snprintf(buf, sizeof(buf), "AFI %s (%u), no AF printer !",
                 tok2str(af_values, "Unknown", af), af);
    } else {
        snprintf(buf, sizeof(buf), "AFI %s (%u): %s",
                 tok2str(af_values, "Unknown", af), af, (*pfunc)(ndo, tptr+1));
    }

    return buf;
}
