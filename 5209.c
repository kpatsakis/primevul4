static char const *acurite_getChannelAndType(uint8_t byte, uint8_t mtype)
{
    static char const *channel_strs[] = {"CR", "ER", "BR", "AR", "CF", "EF", "BF", "AF"}; // 'E' stands for error

    int channel = ((mtype & 0x01) << 2) | ((byte & 0xC0) >> 6);
    return channel_strs[channel];
}