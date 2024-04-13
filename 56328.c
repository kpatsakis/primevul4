make_unix_date2(const u_char *date_ptr)
{
    uint32_t x, x2;

    x = EXTRACT_LE_32BITS(date_ptr);
    x2 = ((x & 0xFFFF) << 16) | ((x & 0xFFFF0000) >> 16);
    return int_unix_date(x2);
}
