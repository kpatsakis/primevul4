static u16 vhost_scsi_get_tpgt(struct se_portal_group *se_tpg)
{
	struct vhost_scsi_tpg *tpg = container_of(se_tpg,
				struct vhost_scsi_tpg, se_tpg);
	return tpg->tport_tpgt;
}
