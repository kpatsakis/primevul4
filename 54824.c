static int uas_target_alloc(struct scsi_target *starget)
{
	struct uas_dev_info *devinfo = (struct uas_dev_info *)
			dev_to_shost(starget->dev.parent)->hostdata;

	if (devinfo->flags & US_FL_NO_REPORT_LUNS)
		starget->no_report_luns = 1;

	return 0;
}
