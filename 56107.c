format_interval(const uint32_t n)
{
    static char buf[4][sizeof("0000000.000s")];
    static int i = 0;
    i = (i + 1) % 4;
    snprintf(buf[i], sizeof(buf[i]), "%u.%03us", n / 1000, n % 1000);
    return buf[i];
}
