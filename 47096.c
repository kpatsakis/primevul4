static void vhost_scsi_drop_tport(struct se_wwn *wwn)
{
	struct vhost_scsi_tport *tport = container_of(wwn,
				struct vhost_scsi_tport, tport_wwn);

	pr_debug("TCM_VHost_ConfigFS: Deallocating emulated Target"
		" %s Address: %s\n", vhost_scsi_dump_proto_id(tport),
		tport->tport_name);

	kfree(tport);
}
