lldp_print(netdissect_options *ndo,
           register const u_char *pptr, register u_int len)
{
    uint8_t subtype;
    uint16_t tlv, cap, ena_cap;
    u_int oui, tlen, hexdump, tlv_type, tlv_len;
    const u_char *tptr;
    char *network_addr;

    tptr = pptr;
    tlen = len;

    ND_PRINT((ndo, "LLDP, length %u", len));

    while (tlen >= sizeof(tlv)) {

        ND_TCHECK2(*tptr, sizeof(tlv));

        tlv = EXTRACT_16BITS(tptr);

        tlv_type = LLDP_EXTRACT_TYPE(tlv);
        tlv_len = LLDP_EXTRACT_LEN(tlv);
        hexdump = FALSE;

        tlen -= sizeof(tlv);
        tptr += sizeof(tlv);

        if (ndo->ndo_vflag) {
            ND_PRINT((ndo, "\n\t%s TLV (%u), length %u",
                   tok2str(lldp_tlv_values, "Unknown", tlv_type),
                   tlv_type, tlv_len));
        }

        /* infinite loop check */
        if (!tlv_type || !tlv_len) {
            break;
        }

        ND_TCHECK2(*tptr, tlv_len);
        if (tlen < tlv_len) {
            goto trunc;
        }

        switch (tlv_type) {

        case LLDP_CHASSIS_ID_TLV:
            if (ndo->ndo_vflag) {
                if (tlv_len < 2) {
                    goto trunc;
                }
                subtype = *tptr;
                ND_PRINT((ndo, "\n\t  Subtype %s (%u): ",
                       tok2str(lldp_chassis_subtype_values, "Unknown", subtype),
                       subtype));

                switch (subtype) {
                case LLDP_CHASSIS_MAC_ADDR_SUBTYPE:
                    if (tlv_len < 1+6) {
                        goto trunc;
                    }
                    ND_PRINT((ndo, "%s", etheraddr_string(ndo, tptr + 1)));
                    break;

                case LLDP_CHASSIS_INTF_NAME_SUBTYPE: /* fall through */
                case LLDP_CHASSIS_LOCAL_SUBTYPE:
                case LLDP_CHASSIS_CHASSIS_COMP_SUBTYPE:
                case LLDP_CHASSIS_INTF_ALIAS_SUBTYPE:
                case LLDP_CHASSIS_PORT_COMP_SUBTYPE:
                    safeputs(ndo, tptr + 1, tlv_len - 1);
                    break;

                case LLDP_CHASSIS_NETWORK_ADDR_SUBTYPE:
                    network_addr = lldp_network_addr_print(ndo, tptr+1, tlv_len-1);
                    if (network_addr == NULL) {
                        goto trunc;
                    }
                    ND_PRINT((ndo, "%s", network_addr));
                    break;

                default:
                    hexdump = TRUE;
                    break;
                }
            }
            break;

        case LLDP_PORT_ID_TLV:
            if (ndo->ndo_vflag) {
                if (tlv_len < 2) {
                    goto trunc;
                }
                subtype = *tptr;
                ND_PRINT((ndo, "\n\t  Subtype %s (%u): ",
                       tok2str(lldp_port_subtype_values, "Unknown", subtype),
                       subtype));

                switch (subtype) {
                case LLDP_PORT_MAC_ADDR_SUBTYPE:
                    if (tlv_len < 1+6) {
                        goto trunc;
                    }
                    ND_PRINT((ndo, "%s", etheraddr_string(ndo, tptr + 1)));
                    break;

                case LLDP_PORT_INTF_NAME_SUBTYPE: /* fall through */
                case LLDP_PORT_LOCAL_SUBTYPE:
                case LLDP_PORT_AGENT_CIRC_ID_SUBTYPE:
                case LLDP_PORT_INTF_ALIAS_SUBTYPE:
                case LLDP_PORT_PORT_COMP_SUBTYPE:
                    safeputs(ndo, tptr + 1, tlv_len - 1);
                    break;

                case LLDP_PORT_NETWORK_ADDR_SUBTYPE:
                    network_addr = lldp_network_addr_print(ndo, tptr+1, tlv_len-1);
                    if (network_addr == NULL) {
                        goto trunc;
                    }
                    ND_PRINT((ndo, "%s", network_addr));
                    break;

                default:
                    hexdump = TRUE;
                    break;
                }
            }
            break;

        case LLDP_TTL_TLV:
            if (ndo->ndo_vflag) {
                if (tlv_len < 2) {
                    goto trunc;
                }
                ND_PRINT((ndo, ": TTL %us", EXTRACT_16BITS(tptr)));
            }
            break;

        case LLDP_PORT_DESCR_TLV:
            if (ndo->ndo_vflag) {
                ND_PRINT((ndo, ": "));
                safeputs(ndo, tptr, tlv_len);
            }
            break;

        case LLDP_SYSTEM_NAME_TLV:
            /*
             * The system name is also print in non-verbose mode
             * similar to the CDP printer.
             */
            ND_PRINT((ndo, ": "));
            safeputs(ndo, tptr, tlv_len);
            break;

        case LLDP_SYSTEM_DESCR_TLV:
            if (ndo->ndo_vflag) {
                ND_PRINT((ndo, "\n\t  "));
                safeputs(ndo, tptr, tlv_len);
            }
            break;

        case LLDP_SYSTEM_CAP_TLV:
            if (ndo->ndo_vflag) {
                /*
                 * XXX - IEEE Std 802.1AB-2009 says the first octet
                 * if a chassis ID subtype, with the system
                 * capabilities and enabled capabilities following
                 * it.
                 */
                if (tlv_len < 4) {
                    goto trunc;
                }
                cap = EXTRACT_16BITS(tptr);
                ena_cap = EXTRACT_16BITS(tptr+2);
                ND_PRINT((ndo, "\n\t  System  Capabilities [%s] (0x%04x)",
                       bittok2str(lldp_cap_values, "none", cap), cap));
                ND_PRINT((ndo, "\n\t  Enabled Capabilities [%s] (0x%04x)",
                       bittok2str(lldp_cap_values, "none", ena_cap), ena_cap));
            }
            break;

        case LLDP_MGMT_ADDR_TLV:
            if (ndo->ndo_vflag) {
                if (!lldp_mgmt_addr_tlv_print(ndo, tptr, tlv_len)) {
                    goto trunc;
                }
            }
            break;

        case LLDP_PRIVATE_TLV:
            if (ndo->ndo_vflag) {
                if (tlv_len < 3) {
                    goto trunc;
                }
                oui = EXTRACT_24BITS(tptr);
                ND_PRINT((ndo, ": OUI %s (0x%06x)", tok2str(oui_values, "Unknown", oui), oui));

                switch (oui) {
                case OUI_IEEE_8021_PRIVATE:
                    hexdump = lldp_private_8021_print(ndo, tptr, tlv_len);
                    break;
                case OUI_IEEE_8023_PRIVATE:
                    hexdump = lldp_private_8023_print(ndo, tptr, tlv_len);
                    break;
		case OUI_IANA:
                    hexdump = lldp_private_iana_print(ndo, tptr, tlv_len);
                    break;
                case OUI_TIA:
                    hexdump = lldp_private_tia_print(ndo, tptr, tlv_len);
                    break;
                case OUI_DCBX:
                    hexdump = lldp_private_dcbx_print(ndo, tptr, tlv_len);
                    break;
                default:
                    hexdump = TRUE;
                    break;
                }
            }
            break;

        default:
            hexdump = TRUE;
            break;
        }

        /* do we also want to see a hex dump ? */
        if (ndo->ndo_vflag > 1 || (ndo->ndo_vflag && hexdump)) {
            print_unknown_data(ndo, tptr, "\n\t  ", tlv_len);
        }

        tlen -= tlv_len;
        tptr += tlv_len;
    }
    return;
 trunc:
    ND_PRINT((ndo, "\n\t[|LLDP]"));
}
