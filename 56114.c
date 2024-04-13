print_type_in_line(netdissect_options *ndo,
                   uint32_t type, int count, int indent, int *first_one)
{
    if (count > 0) {
        if (*first_one) {
            *first_one = 0;
            if (indent > 1) {
                u_int t;
                ND_PRINT((ndo, "\n"));
                for (t = indent; t > 0; t--)
                    ND_PRINT((ndo, "\t"));
            } else {
                ND_PRINT((ndo, " "));
            }
        } else {
            ND_PRINT((ndo, ", "));
        }
        ND_PRINT((ndo, "%s", tok2str(type_values, "Easter Egg", type)));
        if (count > 1)
            ND_PRINT((ndo, " (x%d)", count));
    }
}
