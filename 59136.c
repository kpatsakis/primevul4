copy_guid_from_buf (GUID* guid, unsigned char *buf, size_t len)
{
    int i;
    int idx = 0;
    assert (guid);
    assert (buf);

    CHECKINT32(idx, len); guid->data1 = GETINT32(buf + idx); idx += sizeof (uint32);
    CHECKINT16(idx, len); guid->data2 = GETINT16(buf + idx); idx += sizeof (uint16);
    CHECKINT16(idx, len); guid->data3 = GETINT16(buf + idx); idx += sizeof (uint16);
    for (i = 0; i < 8; i++, idx += sizeof (uint8))
	guid->data4[i] = (uint8)(buf[idx]);
}
