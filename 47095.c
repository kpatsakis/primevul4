static void vhost_scsi_drop_nodeacl(struct se_node_acl *se_acl)
{
	struct vhost_scsi_nacl *nacl = container_of(se_acl,
				struct vhost_scsi_nacl, se_node_acl);
	core_tpg_del_initiator_node_acl(se_acl->se_tpg, se_acl, 1);
	kfree(nacl);
}
