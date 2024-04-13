static int uas_slave_configure(struct scsi_device *sdev)
{
	struct uas_dev_info *devinfo = sdev->hostdata;

	if (devinfo->flags & US_FL_NO_REPORT_OPCODES)
		sdev->no_report_opcodes = 1;

	/* A few buggy USB-ATA bridges don't understand FUA */
	if (devinfo->flags & US_FL_BROKEN_FUA)
		sdev->broken_fua = 1;

	scsi_change_queue_depth(sdev, devinfo->qdepth - 2);
	return 0;
}
