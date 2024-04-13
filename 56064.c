print_ets_priority_assignment_table(netdissect_options *ndo,
                                    const u_char *ptr)
{
    ND_PRINT((ndo, "\n\t    Priority Assignment Table"));
    ND_PRINT((ndo, "\n\t     Priority : 0   1   2   3   4   5   6   7"));
    ND_PRINT((ndo, "\n\t     Value    : %-3d %-3d %-3d %-3d %-3d %-3d %-3d %-3d",
            ptr[0]>>4,ptr[0]&0x0f,ptr[1]>>4,ptr[1]&0x0f,ptr[2]>>4,
            ptr[2] & 0x0f, ptr[3] >> 4, ptr[3] & 0x0f));
}
