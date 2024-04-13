osi_print_cksum(netdissect_options *ndo, const uint8_t *pptr,
	        uint16_t checksum, int checksum_offset, u_int length)
{
        uint16_t calculated_checksum;

        /* do not attempt to verify the checksum if it is zero,
         * if the offset is nonsense,
         * or the base pointer is not sane
         */
        if (!checksum
            || checksum_offset < 0
            || !ND_TTEST2(*(pptr + checksum_offset), 2)
            || (u_int)checksum_offset > length
            || !ND_TTEST2(*pptr, length)) {
                ND_PRINT((ndo, " (unverified)"));
        } else {
#if 0
                printf("\nosi_print_cksum: %p %u %u\n", pptr, checksum_offset, length);
#endif
                calculated_checksum = create_osi_cksum(pptr, checksum_offset, length);
                if (checksum == calculated_checksum) {
                        ND_PRINT((ndo, " (correct)"));
                } else {
                        ND_PRINT((ndo, " (incorrect should be 0x%04x)", calculated_checksum));
                }
        }
}
