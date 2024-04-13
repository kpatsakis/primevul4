is_in_line(netdissect_options *ndo, int indent)
{
    return indent - 1 >= ndo->ndo_vflag && ndo->ndo_vflag < 3;
}
