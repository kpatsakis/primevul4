vhost_scsi_release_fabric_acl(struct se_portal_group *se_tpg,
			     struct se_node_acl *se_nacl)
{
	struct vhost_scsi_nacl *nacl = container_of(se_nacl,
			struct vhost_scsi_nacl, se_node_acl);
	kfree(nacl);
}
