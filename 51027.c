static int arcmsr_bios_param(struct scsi_device *sdev,
		struct block_device *bdev, sector_t capacity, int *geom)
{
	int ret, heads, sectors, cylinders, total_capacity;
	unsigned char *buffer;/* return copy of block device's partition table */

	buffer = scsi_bios_ptable(bdev);
	if (buffer) {
		ret = scsi_partsize(buffer, capacity, &geom[2], &geom[0], &geom[1]);
		kfree(buffer);
		if (ret != -1)
			return ret;
	}
	total_capacity = capacity;
	heads = 64;
	sectors = 32;
	cylinders = total_capacity / (heads * sectors);
	if (cylinders > 1024) {
		heads = 255;
		sectors = 63;
		cylinders = total_capacity / (heads * sectors);
	}
	geom[0] = heads;
	geom[1] = sectors;
	geom[2] = cylinders;
	return 0;
}
