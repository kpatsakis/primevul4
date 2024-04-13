clnp_print(netdissect_options *ndo,
           const uint8_t *pptr, u_int length)
{
	const uint8_t *optr,*source_address,*dest_address;
        u_int li,tlen,nsap_offset,source_address_length,dest_address_length, clnp_pdu_type, clnp_flags;
	const struct clnp_header_t *clnp_header;
	const struct clnp_segment_header_t *clnp_segment_header;
        uint8_t rfd_error_major,rfd_error_minor;

	clnp_header = (const struct clnp_header_t *) pptr;
        ND_TCHECK(*clnp_header);

        li = clnp_header->length_indicator;
        optr = pptr;

        if (!ndo->ndo_eflag)
            ND_PRINT((ndo, "CLNP"));

        /*
         * Sanity checking of the header.
         */

        if (clnp_header->version != CLNP_VERSION) {
            ND_PRINT((ndo, "version %d packet not supported", clnp_header->version));
            return (0);
        }

	if (li > length) {
            ND_PRINT((ndo, " length indicator(%u) > PDU size (%u)!", li, length));
            return (0);
	}

        if (li < sizeof(struct clnp_header_t)) {
            ND_PRINT((ndo, " length indicator %u < min PDU size:", li));
            while (pptr < ndo->ndo_snapend)
                ND_PRINT((ndo, "%02X", *pptr++));
            return (0);
        }

        /* FIXME further header sanity checking */

        clnp_pdu_type = clnp_header->type & CLNP_PDU_TYPE_MASK;
        clnp_flags = clnp_header->type & CLNP_FLAG_MASK;

        pptr += sizeof(struct clnp_header_t);
        li -= sizeof(struct clnp_header_t);

        if (li < 1) {
            ND_PRINT((ndo, "li < size of fixed part of CLNP header and addresses"));
            return (0);
        }
	ND_TCHECK(*pptr);
        dest_address_length = *pptr;
        pptr += 1;
        li -= 1;
        if (li < dest_address_length) {
            ND_PRINT((ndo, "li < size of fixed part of CLNP header and addresses"));
            return (0);
        }
        ND_TCHECK2(*pptr, dest_address_length);
        dest_address = pptr;
        pptr += dest_address_length;
        li -= dest_address_length;

        if (li < 1) {
            ND_PRINT((ndo, "li < size of fixed part of CLNP header and addresses"));
            return (0);
        }
	ND_TCHECK(*pptr);
        source_address_length = *pptr;
        pptr += 1;
        li -= 1;
        if (li < source_address_length) {
            ND_PRINT((ndo, "li < size of fixed part of CLNP header and addresses"));
            return (0);
        }
        ND_TCHECK2(*pptr, source_address_length);
        source_address = pptr;
        pptr += source_address_length;
        li -= source_address_length;

        if (ndo->ndo_vflag < 1) {
            ND_PRINT((ndo, "%s%s > %s, %s, length %u",
                   ndo->ndo_eflag ? "" : ", ",
                   isonsap_string(ndo, source_address, source_address_length),
                   isonsap_string(ndo, dest_address, dest_address_length),
                   tok2str(clnp_pdu_values,"unknown (%u)",clnp_pdu_type),
                   length));
            return (1);
        }
        ND_PRINT((ndo, "%slength %u", ndo->ndo_eflag ? "" : ", ", length));

        ND_PRINT((ndo, "\n\t%s PDU, hlen: %u, v: %u, lifetime: %u.%us, Segment PDU length: %u, checksum: 0x%04x",
               tok2str(clnp_pdu_values, "unknown (%u)",clnp_pdu_type),
               clnp_header->length_indicator,
               clnp_header->version,
               clnp_header->lifetime/2,
               (clnp_header->lifetime%2)*5,
               EXTRACT_16BITS(clnp_header->segment_length),
               EXTRACT_16BITS(clnp_header->cksum)));

        osi_print_cksum(ndo, optr, EXTRACT_16BITS(clnp_header->cksum), 7,
                        clnp_header->length_indicator);

        ND_PRINT((ndo, "\n\tFlags [%s]",
               bittok2str(clnp_flag_values, "none", clnp_flags)));

        ND_PRINT((ndo, "\n\tsource address (length %u): %s\n\tdest   address (length %u): %s",
               source_address_length,
               isonsap_string(ndo, source_address, source_address_length),
               dest_address_length,
               isonsap_string(ndo, dest_address, dest_address_length)));

        if (clnp_flags & CLNP_SEGMENT_PART) {
                if (li < sizeof(const struct clnp_segment_header_t)) {
                    ND_PRINT((ndo, "li < size of fixed part of CLNP header, addresses, and segment part"));
                    return (0);
                }
            	clnp_segment_header = (const struct clnp_segment_header_t *) pptr;
                ND_TCHECK(*clnp_segment_header);
                ND_PRINT((ndo, "\n\tData Unit ID: 0x%04x, Segment Offset: %u, Total PDU Length: %u",
                       EXTRACT_16BITS(clnp_segment_header->data_unit_id),
                       EXTRACT_16BITS(clnp_segment_header->segment_offset),
                       EXTRACT_16BITS(clnp_segment_header->total_length)));
                pptr+=sizeof(const struct clnp_segment_header_t);
                li-=sizeof(const struct clnp_segment_header_t);
        }

        /* now walk the options */
        while (li >= 2) {
            u_int op, opli;
            const uint8_t *tptr;

            if (li < 2) {
                ND_PRINT((ndo, ", bad opts/li"));
                return (0);
            }
            ND_TCHECK2(*pptr, 2);
            op = *pptr++;
            opli = *pptr++;
            li -= 2;
            if (opli > li) {
                ND_PRINT((ndo, ", opt (%d) too long", op));
                return (0);
            }
            ND_TCHECK2(*pptr, opli);
            li -= opli;
            tptr = pptr;
            tlen = opli;

            ND_PRINT((ndo, "\n\t  %s Option #%u, length %u, value: ",
                   tok2str(clnp_option_values,"Unknown",op),
                   op,
                   opli));

            /*
             * We've already checked that the entire option is present
             * in the captured packet with the ND_TCHECK2() call.
             * Therefore, we don't need to do ND_TCHECK()/ND_TCHECK2()
             * checks.
             * We do, however, need to check tlen, to make sure we
             * don't run past the end of the option.
	     */
            switch (op) {


            case CLNP_OPTION_ROUTE_RECORDING: /* those two options share the format */
            case CLNP_OPTION_SOURCE_ROUTING:
                    if (tlen < 2) {
                            ND_PRINT((ndo, ", bad opt len"));
                            return (0);
                    }
                    ND_PRINT((ndo, "%s %s",
                           tok2str(clnp_option_sr_rr_values,"Unknown",*tptr),
                           tok2str(clnp_option_sr_rr_string_values, "Unknown Option %u", op)));
                    nsap_offset=*(tptr+1);
                    if (nsap_offset == 0) {
                            ND_PRINT((ndo, " Bad NSAP offset (0)"));
                            break;
                    }
                    nsap_offset-=1; /* offset to nsap list */
                    if (nsap_offset > tlen) {
                            ND_PRINT((ndo, " Bad NSAP offset (past end of option)"));
                            break;
                    }
                    tptr+=nsap_offset;
                    tlen-=nsap_offset;
                    while (tlen > 0) {
                            source_address_length=*tptr;
                            if (tlen < source_address_length+1) {
                                    ND_PRINT((ndo, "\n\t    NSAP address goes past end of option"));
                                    break;
                            }
                            if (source_address_length > 0) {
                                    source_address=(tptr+1);
                                    ND_TCHECK2(*source_address, source_address_length);
                                    ND_PRINT((ndo, "\n\t    NSAP address (length %u): %s",
                                           source_address_length,
                                           isonsap_string(ndo, source_address, source_address_length)));
                            }
                            tlen-=source_address_length+1;
                    }
                    break;

            case CLNP_OPTION_PRIORITY:
                    if (tlen < 1) {
                            ND_PRINT((ndo, ", bad opt len"));
                            return (0);
                    }
                    ND_PRINT((ndo, "0x%1x", *tptr&0x0f));
                    break;

            case CLNP_OPTION_QOS_MAINTENANCE:
                    if (tlen < 1) {
                            ND_PRINT((ndo, ", bad opt len"));
                            return (0);
                    }
                    ND_PRINT((ndo, "\n\t    Format Code: %s",
                           tok2str(clnp_option_scope_values, "Reserved", *tptr&CLNP_OPTION_SCOPE_MASK)));

                    if ((*tptr&CLNP_OPTION_SCOPE_MASK) == CLNP_OPTION_SCOPE_GLOBAL)
                            ND_PRINT((ndo, "\n\t    QoS Flags [%s]",
                                   bittok2str(clnp_option_qos_global_values,
                                              "none",
                                              *tptr&CLNP_OPTION_OPTION_QOS_MASK)));
                    break;

            case CLNP_OPTION_SECURITY:
                    if (tlen < 2) {
                            ND_PRINT((ndo, ", bad opt len"));
                            return (0);
                    }
                    ND_PRINT((ndo, "\n\t    Format Code: %s, Security-Level %u",
                           tok2str(clnp_option_scope_values,"Reserved",*tptr&CLNP_OPTION_SCOPE_MASK),
                           *(tptr+1)));
                    break;

            case CLNP_OPTION_DISCARD_REASON:
                if (tlen < 1) {
                        ND_PRINT((ndo, ", bad opt len"));
                        return (0);
                }
                rfd_error_major = (*tptr&0xf0) >> 4;
                rfd_error_minor = *tptr&0x0f;
                ND_PRINT((ndo, "\n\t    Class: %s Error (0x%01x), %s (0x%01x)",
                       tok2str(clnp_option_rfd_class_values,"Unknown",rfd_error_major),
                       rfd_error_major,
                       tok2str(clnp_option_rfd_error_class[rfd_error_major],"Unknown",rfd_error_minor),
                       rfd_error_minor));
                break;

            case CLNP_OPTION_PADDING:
                    ND_PRINT((ndo, "padding data"));
                break;

                /*
                 * FIXME those are the defined Options that lack a decoder
                 * you are welcome to contribute code ;-)
                 */

            default:
                print_unknown_data(ndo, tptr, "\n\t  ", opli);
                break;
            }
            if (ndo->ndo_vflag > 1)
                print_unknown_data(ndo, pptr, "\n\t  ", opli);
            pptr += opli;
        }

        switch (clnp_pdu_type) {

        case    CLNP_PDU_ER: /* fall through */
        case 	CLNP_PDU_ERP:
            ND_TCHECK(*pptr);
            if (*(pptr) == NLPID_CLNP) {
                ND_PRINT((ndo, "\n\t-----original packet-----\n\t"));
                /* FIXME recursion protection */
                clnp_print(ndo, pptr, length - clnp_header->length_indicator);
                break;
            }

        case 	CLNP_PDU_DT:
        case 	CLNP_PDU_MD:
        case 	CLNP_PDU_ERQ:

        default:
            /* dump the PDU specific data */
            if (length-(pptr-optr) > 0) {
                ND_PRINT((ndo, "\n\t  undecoded non-header data, length %u", length-clnp_header->length_indicator));
                print_unknown_data(ndo, pptr, "\n\t  ", length - (pptr - optr));
            }
        }

        return (1);

 trunc:
    ND_PRINT((ndo, "[|clnp]"));
    return (1);

}
