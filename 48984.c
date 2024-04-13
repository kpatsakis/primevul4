fromhex(const char *hexstr, unsigned char *out)
{
    const char *p;
    size_t count;

    for (p = hexstr, count = 0; *p != '\0'; p += 2, count++)
        sscanf(p, "%2hhx", &out[count]);
    return count;
}
