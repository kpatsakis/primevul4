static char *vhost_scsi_get_fabric_wwn(struct se_portal_group *se_tpg)
{
	struct vhost_scsi_tpg *tpg = container_of(se_tpg,
				struct vhost_scsi_tpg, se_tpg);
	struct vhost_scsi_tport *tport = tpg->tport;

	return &tport->tport_name[0];
}
