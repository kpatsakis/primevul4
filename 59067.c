do_checksum_math(u_int16_t *data, int len)
{
    int sum = 0;
    union {
        u_int16_t s;
        u_int8_t b[2];
    } pad;

    while (len > 1) {
        sum += *data++;
        len -= 2;
    }

    if (len == 1) {
        pad.b[0] = *(u_int8_t *)data;
        pad.b[1] = 0;
        sum += pad.s;
    }

    return (sum);
}
