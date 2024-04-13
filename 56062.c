lldp_private_iana_print(netdissect_options *ndo,
                        const u_char *tptr, u_int tlv_len)
{
    int subtype, hexdump = FALSE;

    if (tlv_len < 8) {
        return hexdump;
    }
    subtype = *(tptr+3);

    ND_PRINT((ndo, "\n\t  %s Subtype (%u)",
           tok2str(lldp_iana_subtype_values, "unknown", subtype),
           subtype));

    switch (subtype) {
    case LLDP_IANA_SUBTYPE_MUDURL:
        ND_PRINT((ndo, "\n\t  MUD-URL="));
        (void)fn_printn(ndo, tptr+4, tlv_len-4, NULL);
        break;
    default:
        hexdump=TRUE;
    }

    return hexdump;
}
