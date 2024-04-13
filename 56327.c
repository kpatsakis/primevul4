make_unix_date(const u_char *date_ptr)
{
    uint32_t dos_date = 0;

    dos_date = EXTRACT_LE_32BITS(date_ptr);

    return int_unix_date(dos_date);
}
