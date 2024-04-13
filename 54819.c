static int uas_slave_alloc(struct scsi_device *sdev)
{
	struct uas_dev_info *devinfo =
		(struct uas_dev_info *)sdev->host->hostdata;

	sdev->hostdata = devinfo;

	/*
	 * USB has unusual DMA-alignment requirements: Although the
	 * starting address of each scatter-gather element doesn't matter,
	 * the length of each element except the last must be divisible
	 * by the Bulk maxpacket value.  There's currently no way to
	 * express this by block-layer constraints, so we'll cop out
	 * and simply require addresses to be aligned at 512-byte
	 * boundaries.  This is okay since most block I/O involves
	 * hardware sectors that are multiples of 512 bytes in length,
	 * and since host controllers up through USB 2.0 have maxpacket
	 * values no larger than 512.
	 *
	 * But it doesn't suffice for Wireless USB, where Bulk maxpacket
	 * values can be as large as 2048.  To make that work properly
	 * will require changes to the block layer.
	 */
	blk_queue_update_dma_alignment(sdev->request_queue, (512 - 1));

	if (devinfo->flags & US_FL_MAX_SECTORS_64)
		blk_queue_max_hw_sectors(sdev->request_queue, 64);
	else if (devinfo->flags & US_FL_MAX_SECTORS_240)
		blk_queue_max_hw_sectors(sdev->request_queue, 240);

	return 0;
}
