lldp_private_tia_print(netdissect_options *ndo,
                       const u_char *tptr, u_int tlv_len)
{
    int subtype, hexdump = FALSE;
    uint8_t location_format;
    uint16_t power_val;
    u_int lci_len;
    uint8_t ca_type, ca_len;

    if (tlv_len < 4) {
        return hexdump;
    }
    subtype = *(tptr+3);

    ND_PRINT((ndo, "\n\t  %s Subtype (%u)",
           tok2str(lldp_tia_subtype_values, "unknown", subtype),
           subtype));

    switch (subtype) {
    case LLDP_PRIVATE_TIA_SUBTYPE_CAPABILITIES:
        if (tlv_len < 7) {
            return hexdump;
        }
        ND_PRINT((ndo, "\n\t    Media capabilities [%s] (0x%04x)",
               bittok2str(lldp_tia_capabilities_values, "none",
                          EXTRACT_16BITS(tptr + 4)), EXTRACT_16BITS(tptr + 4)));
        ND_PRINT((ndo, "\n\t    Device type [%s] (0x%02x)",
               tok2str(lldp_tia_device_type_values, "unknown", *(tptr+6)),
               *(tptr + 6)));
        break;

    case LLDP_PRIVATE_TIA_SUBTYPE_NETWORK_POLICY:
        if (tlv_len < 8) {
            return hexdump;
        }
        ND_PRINT((ndo, "\n\t    Application type [%s] (0x%02x)",
               tok2str(lldp_tia_application_type_values, "none", *(tptr+4)),
               *(tptr + 4)));
        ND_PRINT((ndo, ", Flags [%s]", bittok2str(
                   lldp_tia_network_policy_bits_values, "none", *(tptr + 5))));
        ND_PRINT((ndo, "\n\t    Vlan id %u",
               LLDP_EXTRACT_NETWORK_POLICY_VLAN(EXTRACT_16BITS(tptr + 5))));
        ND_PRINT((ndo, ", L2 priority %u",
               LLDP_EXTRACT_NETWORK_POLICY_L2_PRIORITY(EXTRACT_16BITS(tptr + 6))));
        ND_PRINT((ndo, ", DSCP value %u",
               LLDP_EXTRACT_NETWORK_POLICY_DSCP(EXTRACT_16BITS(tptr + 6))));
        break;

    case LLDP_PRIVATE_TIA_SUBTYPE_LOCAL_ID:
        if (tlv_len < 5) {
            return hexdump;
        }
        location_format = *(tptr+4);
        ND_PRINT((ndo, "\n\t    Location data format %s (0x%02x)",
               tok2str(lldp_tia_location_data_format_values, "unknown", location_format),
               location_format));

        switch (location_format) {
        case LLDP_TIA_LOCATION_DATA_FORMAT_COORDINATE_BASED:
            if (tlv_len < 21) {
                return hexdump;
            }
            ND_PRINT((ndo, "\n\t    Latitude resolution %u, latitude value %" PRIu64,
                   (*(tptr + 5) >> 2), lldp_extract_latlon(tptr + 5)));
            ND_PRINT((ndo, "\n\t    Longitude resolution %u, longitude value %" PRIu64,
                   (*(tptr + 10) >> 2), lldp_extract_latlon(tptr + 10)));
            ND_PRINT((ndo, "\n\t    Altitude type %s (%u)",
                   tok2str(lldp_tia_location_altitude_type_values, "unknown",(*(tptr+15)>>4)),
                   (*(tptr + 15) >> 4)));
            ND_PRINT((ndo, "\n\t    Altitude resolution %u, altitude value 0x%x",
                   (EXTRACT_16BITS(tptr+15)>>6)&0x3f,
                   ((EXTRACT_32BITS(tptr + 16) & 0x3fffffff))));
            ND_PRINT((ndo, "\n\t    Datum %s (0x%02x)",
                   tok2str(lldp_tia_location_datum_type_values, "unknown", *(tptr+20)),
                   *(tptr + 20)));
            break;

        case LLDP_TIA_LOCATION_DATA_FORMAT_CIVIC_ADDRESS:
            if (tlv_len < 6) {
                return hexdump;
            }
            lci_len = *(tptr+5);
            if (lci_len < 3) {
                return hexdump;
            }
            if (tlv_len < 7+lci_len) {
                return hexdump;
            }
            ND_PRINT((ndo, "\n\t    LCI length %u, LCI what %s (0x%02x), Country-code ",
                   lci_len,
                   tok2str(lldp_tia_location_lci_what_values, "unknown", *(tptr+6)),
                   *(tptr + 6)));

            /* Country code */
            safeputs(ndo, tptr + 7, 2);

            lci_len = lci_len-3;
            tptr = tptr + 9;

            /* Decode each civic address element */
            while (lci_len > 0) {
                if (lci_len < 2) {
                    return hexdump;
                }
		ca_type = *(tptr);
                ca_len = *(tptr+1);

		tptr += 2;
                lci_len -= 2;

                ND_PRINT((ndo, "\n\t      CA type \'%s\' (%u), length %u: ",
                       tok2str(lldp_tia_location_lci_catype_values, "unknown", ca_type),
                       ca_type, ca_len));

		/* basic sanity check */
		if ( ca_type == 0 || ca_len == 0) {
                    return hexdump;
		}
		if (lci_len < ca_len) {
		    return hexdump;
		}

                safeputs(ndo, tptr, ca_len);
                tptr += ca_len;
                lci_len -= ca_len;
            }
            break;

        case LLDP_TIA_LOCATION_DATA_FORMAT_ECS_ELIN:
            ND_PRINT((ndo, "\n\t    ECS ELIN id "));
            safeputs(ndo, tptr + 5, tlv_len - 5);
            break;

        default:
            ND_PRINT((ndo, "\n\t    Location ID "));
            print_unknown_data(ndo, tptr + 5, "\n\t      ", tlv_len - 5);
        }
        break;

    case LLDP_PRIVATE_TIA_SUBTYPE_EXTENDED_POWER_MDI:
        if (tlv_len < 7) {
            return hexdump;
        }
        ND_PRINT((ndo, "\n\t    Power type [%s]",
               (*(tptr + 4) & 0xC0 >> 6) ? "PD device" : "PSE device"));
        ND_PRINT((ndo, ", Power source [%s]",
               tok2str(lldp_tia_power_source_values, "none", (*(tptr + 4) & 0x30) >> 4)));
        ND_PRINT((ndo, "\n\t    Power priority [%s] (0x%02x)",
               tok2str(lldp_tia_power_priority_values, "none", *(tptr+4)&0x0f),
               *(tptr + 4) & 0x0f));
        power_val = EXTRACT_16BITS(tptr+5);
        if (power_val < LLDP_TIA_POWER_VAL_MAX) {
            ND_PRINT((ndo, ", Power %.1f Watts", ((float)power_val) / 10));
        } else {
            ND_PRINT((ndo, ", Power %u (Reserved)", power_val));
        }
        break;

    case LLDP_PRIVATE_TIA_SUBTYPE_INVENTORY_HARDWARE_REV:
    case LLDP_PRIVATE_TIA_SUBTYPE_INVENTORY_FIRMWARE_REV:
    case LLDP_PRIVATE_TIA_SUBTYPE_INVENTORY_SOFTWARE_REV:
    case LLDP_PRIVATE_TIA_SUBTYPE_INVENTORY_SERIAL_NUMBER:
    case LLDP_PRIVATE_TIA_SUBTYPE_INVENTORY_MANUFACTURER_NAME:
    case LLDP_PRIVATE_TIA_SUBTYPE_INVENTORY_MODEL_NAME:
    case LLDP_PRIVATE_TIA_SUBTYPE_INVENTORY_ASSET_ID:
        ND_PRINT((ndo, "\n\t  %s ",
               tok2str(lldp_tia_inventory_values, "unknown", subtype)));
        safeputs(ndo, tptr + 4, tlv_len - 4);
        break;

    default:
        hexdump = TRUE;
        break;
    }

    return hexdump;
}
