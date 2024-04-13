static u8 vhost_scsi_get_fabric_proto_ident(struct se_portal_group *se_tpg)
{
	struct vhost_scsi_tpg *tpg = container_of(se_tpg,
				struct vhost_scsi_tpg, se_tpg);
	struct vhost_scsi_tport *tport = tpg->tport;

	switch (tport->tport_proto_id) {
	case SCSI_PROTOCOL_SAS:
		return sas_get_fabric_proto_ident(se_tpg);
	case SCSI_PROTOCOL_FCP:
		return fc_get_fabric_proto_ident(se_tpg);
	case SCSI_PROTOCOL_ISCSI:
		return iscsi_get_fabric_proto_ident(se_tpg);
	default:
		pr_err("Unknown tport_proto_id: 0x%02x, using"
			" SAS emulation\n", tport->tport_proto_id);
		break;
	}

	return sas_get_fabric_proto_ident(se_tpg);
}
