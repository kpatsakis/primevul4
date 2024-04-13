isis_print_mtid(netdissect_options *ndo,
                const uint8_t *tptr, const char *ident)
{
    if (!ND_TTEST2(*tptr, 2))
        return(0);

    ND_PRINT((ndo, "%s%s",
           ident,
           tok2str(isis_mt_values,
                   "Reserved for IETF Consensus",
                   ISIS_MASK_MTID(EXTRACT_16BITS(tptr)))));

    ND_PRINT((ndo, " Topology (0x%03x), Flags: [%s]",
           ISIS_MASK_MTID(EXTRACT_16BITS(tptr)),
           bittok2str(isis_mt_flag_values, "none",ISIS_MASK_MTFLAGS(EXTRACT_16BITS(tptr)))));

    return(2);
}
