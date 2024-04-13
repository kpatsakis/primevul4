static unsigned short get_tga_ushort(const unsigned char *data)
{
    return (unsigned short)(data[0] | (data[1] << 8));
}
