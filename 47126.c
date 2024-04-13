static int vhost_scsi_port_link(struct se_portal_group *se_tpg,
			       struct se_lun *lun)
{
	struct vhost_scsi_tpg *tpg = container_of(se_tpg,
				struct vhost_scsi_tpg, se_tpg);

	mutex_lock(&vhost_scsi_mutex);

	mutex_lock(&tpg->tv_tpg_mutex);
	tpg->tv_tpg_port_count++;
	mutex_unlock(&tpg->tv_tpg_mutex);

	vhost_scsi_hotplug(tpg, lun);

	mutex_unlock(&vhost_scsi_mutex);

	return 0;
}
