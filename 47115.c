static void vhost_scsi_hotplug(struct vhost_scsi_tpg *tpg, struct se_lun *lun)
{
	vhost_scsi_do_plug(tpg, lun, true);
}
