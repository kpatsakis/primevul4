bgp_route_refresh_print(netdissect_options *ndo,
                        const u_char *pptr, int len)
{
        const struct bgp_route_refresh *bgp_route_refresh_header;

	ND_TCHECK2(pptr[0], BGP_ROUTE_REFRESH_SIZE);

        /* some little sanity checking */
        if (len<BGP_ROUTE_REFRESH_SIZE)
            return;

        bgp_route_refresh_header = (const struct bgp_route_refresh *)pptr;

        ND_PRINT((ndo, "\n\t  AFI %s (%u), SAFI %s (%u)",
               tok2str(af_values,"Unknown",
			  /* this stinks but the compiler pads the structure
			   * weird */
			  EXTRACT_16BITS(&bgp_route_refresh_header->afi)),
               EXTRACT_16BITS(&bgp_route_refresh_header->afi),
               tok2str(bgp_safi_values,"Unknown",
			  bgp_route_refresh_header->safi),
               bgp_route_refresh_header->safi));

        if (ndo->ndo_vflag > 1) {
            ND_TCHECK2(*pptr, len);
            print_unknown_data(ndo, pptr, "\n\t  ", len);
        }

        return;
trunc:
	ND_PRINT((ndo, "[|BGP]"));
}
