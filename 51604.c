isVolumePartition(struct iso9660 *iso9660, const unsigned char *h)
{
	int32_t location;

	/* Type of the Volume Partition Descriptor must be 3. */
	if (h[0] != 3)
		return (0);

	/* Volume Descriptor Version must be 1. */
	if (h[6] != 1)
		return (0);
	/* Unused Field */
	if (h[7] != 0)
		return (0);

	location = archive_le32dec(h + 72);
	if (location <= SYSTEM_AREA_BLOCK ||
	    location >= iso9660->volume_block)
		return (0);
	if ((uint32_t)location != archive_be32dec(h + 76))
		return (0);

	return (1);
}
