fr_hdr_print(netdissect_options *ndo,
             int length, u_int addr_len, u_int dlci, uint8_t *flags, uint16_t nlpid)
{
    if (ndo->ndo_qflag) {
        ND_PRINT((ndo, "Q.922, DLCI %u, length %u: ",
                     dlci,
                     length));
    } else {
        if (nlpid <= 0xff) /* if its smaller than 256 then its a NLPID */
            ND_PRINT((ndo, "Q.922, hdr-len %u, DLCI %u, Flags [%s], NLPID %s (0x%02x), length %u: ",
                         addr_len,
                         dlci,
                         bittok2str(fr_header_flag_values, "none", EXTRACT_32BITS(flags)),
                         tok2str(nlpid_values,"unknown", nlpid),
                         nlpid,
                         length));
        else /* must be an ethertype */
            ND_PRINT((ndo, "Q.922, hdr-len %u, DLCI %u, Flags [%s], cisco-ethertype %s (0x%04x), length %u: ",
                         addr_len,
                         dlci,
                         bittok2str(fr_header_flag_values, "none", EXTRACT_32BITS(flags)),
                         tok2str(ethertype_values, "unknown", nlpid),
                         nlpid,
                         length));
    }
}
