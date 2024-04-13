vhost_scsi_make_nodeacl(struct se_portal_group *se_tpg,
		       struct config_group *group,
		       const char *name)
{
	struct se_node_acl *se_nacl, *se_nacl_new;
	struct vhost_scsi_nacl *nacl;
	u64 wwpn = 0;
	u32 nexus_depth;

	/* vhost_scsi_parse_wwn(name, &wwpn, 1) < 0)
		return ERR_PTR(-EINVAL); */
	se_nacl_new = vhost_scsi_alloc_fabric_acl(se_tpg);
	if (!se_nacl_new)
		return ERR_PTR(-ENOMEM);

	nexus_depth = 1;
	/*
	 * se_nacl_new may be released by core_tpg_add_initiator_node_acl()
	 * when converting a NodeACL from demo mode -> explict
	 */
	se_nacl = core_tpg_add_initiator_node_acl(se_tpg, se_nacl_new,
				name, nexus_depth);
	if (IS_ERR(se_nacl)) {
		vhost_scsi_release_fabric_acl(se_tpg, se_nacl_new);
		return se_nacl;
	}
	/*
	 * Locate our struct vhost_scsi_nacl and set the FC Nport WWPN
	 */
	nacl = container_of(se_nacl, struct vhost_scsi_nacl, se_node_acl);
	nacl->iport_wwpn = wwpn;

	return se_nacl;
}
