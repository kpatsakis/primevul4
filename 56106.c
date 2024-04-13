format_256(const u_char *data)
{
    static char buf[4][sizeof("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef")];
    static int i = 0;
    i = (i + 1) % 4;
    snprintf(buf[i], sizeof(buf[i]), "%016" PRIx64 "%016" PRIx64 "%016" PRIx64 "%016" PRIx64,
         EXTRACT_64BITS(data),
         EXTRACT_64BITS(data + 8),
         EXTRACT_64BITS(data + 16),
         EXTRACT_64BITS(data + 24)
    );
    return buf[i];
}
