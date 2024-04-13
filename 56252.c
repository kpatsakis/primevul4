juniper_ppp_heuristic_guess(netdissect_options *ndo,
                            register const u_char *p, u_int length)
{
    switch(EXTRACT_16BITS(p)) {
    case PPP_IP :
    case PPP_OSI :
    case PPP_MPLS_UCAST :
    case PPP_MPLS_MCAST :
    case PPP_IPCP :
    case PPP_OSICP :
    case PPP_MPLSCP :
    case PPP_LCP :
    case PPP_PAP :
    case PPP_CHAP :
    case PPP_ML :
    case PPP_IPV6 :
    case PPP_IPV6CP :
        ppp_print(ndo, p, length);
        break;

    default:
        return 0; /* did not find a ppp header */
        break;
    }
    return 1; /* we printed a ppp packet */
}
