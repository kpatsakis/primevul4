print_asc(netdissect_options *ndo,
          const unsigned char *buf, int len)
{
    int i;
    for (i = 0; i < len; i++)
        safeputchar(ndo, buf[i]);
}
