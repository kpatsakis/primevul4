vhost_scsi_alloc_fabric_acl(struct se_portal_group *se_tpg)
{
	struct vhost_scsi_nacl *nacl;

	nacl = kzalloc(sizeof(struct vhost_scsi_nacl), GFP_KERNEL);
	if (!nacl) {
		pr_err("Unable to allocate struct vhost_scsi_nacl\n");
		return NULL;
	}

	return &nacl->se_node_acl;
}
