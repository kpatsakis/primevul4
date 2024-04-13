isNull(struct iso9660 *iso9660, const unsigned char *h, unsigned offset,
unsigned bytes)
{

	while (bytes >= sizeof(iso9660->null)) {
		if (!memcmp(iso9660->null, h + offset, sizeof(iso9660->null)))
			return (0);
		offset += sizeof(iso9660->null);
		bytes -= sizeof(iso9660->null);
	}
	if (bytes)
		return memcmp(iso9660->null, h + offset, bytes) == 0;
	else
		return (1);
}
