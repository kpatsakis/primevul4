static int vhost_scsi_make_nexus(struct vhost_scsi_tpg *tpg,
				const char *name)
{
	struct se_portal_group *se_tpg;
	struct se_session *se_sess;
	struct vhost_scsi_nexus *tv_nexus;
	struct vhost_scsi_cmd *tv_cmd;
	unsigned int i;

	mutex_lock(&tpg->tv_tpg_mutex);
	if (tpg->tpg_nexus) {
		mutex_unlock(&tpg->tv_tpg_mutex);
		pr_debug("tpg->tpg_nexus already exists\n");
		return -EEXIST;
	}
	se_tpg = &tpg->se_tpg;

	tv_nexus = kzalloc(sizeof(struct vhost_scsi_nexus), GFP_KERNEL);
	if (!tv_nexus) {
		mutex_unlock(&tpg->tv_tpg_mutex);
		pr_err("Unable to allocate struct vhost_scsi_nexus\n");
		return -ENOMEM;
	}
	/*
	 *  Initialize the struct se_session pointer and setup tagpool
	 *  for struct vhost_scsi_cmd descriptors
	 */
	tv_nexus->tvn_se_sess = transport_init_session_tags(
					VHOST_SCSI_DEFAULT_TAGS,
					sizeof(struct vhost_scsi_cmd),
					TARGET_PROT_DIN_PASS | TARGET_PROT_DOUT_PASS);
	if (IS_ERR(tv_nexus->tvn_se_sess)) {
		mutex_unlock(&tpg->tv_tpg_mutex);
		kfree(tv_nexus);
		return -ENOMEM;
	}
	se_sess = tv_nexus->tvn_se_sess;
	for (i = 0; i < VHOST_SCSI_DEFAULT_TAGS; i++) {
		tv_cmd = &((struct vhost_scsi_cmd *)se_sess->sess_cmd_map)[i];

		tv_cmd->tvc_sgl = kzalloc(sizeof(struct scatterlist) *
					VHOST_SCSI_PREALLOC_SGLS, GFP_KERNEL);
		if (!tv_cmd->tvc_sgl) {
			mutex_unlock(&tpg->tv_tpg_mutex);
			pr_err("Unable to allocate tv_cmd->tvc_sgl\n");
			goto out;
		}

		tv_cmd->tvc_upages = kzalloc(sizeof(struct page *) *
					VHOST_SCSI_PREALLOC_UPAGES, GFP_KERNEL);
		if (!tv_cmd->tvc_upages) {
			mutex_unlock(&tpg->tv_tpg_mutex);
			pr_err("Unable to allocate tv_cmd->tvc_upages\n");
			goto out;
		}

		tv_cmd->tvc_prot_sgl = kzalloc(sizeof(struct scatterlist) *
					VHOST_SCSI_PREALLOC_PROT_SGLS, GFP_KERNEL);
		if (!tv_cmd->tvc_prot_sgl) {
			mutex_unlock(&tpg->tv_tpg_mutex);
			pr_err("Unable to allocate tv_cmd->tvc_prot_sgl\n");
			goto out;
		}
	}
	/*
	 * Since we are running in 'demo mode' this call with generate a
	 * struct se_node_acl for the vhost_scsi struct se_portal_group with
	 * the SCSI Initiator port name of the passed configfs group 'name'.
	 */
	tv_nexus->tvn_se_sess->se_node_acl = core_tpg_check_initiator_node_acl(
				se_tpg, (unsigned char *)name);
	if (!tv_nexus->tvn_se_sess->se_node_acl) {
		mutex_unlock(&tpg->tv_tpg_mutex);
		pr_debug("core_tpg_check_initiator_node_acl() failed"
				" for %s\n", name);
		goto out;
	}
	/*
	 * Now register the TCM vhost virtual I_T Nexus as active with the
	 * call to __transport_register_session()
	 */
	__transport_register_session(se_tpg, tv_nexus->tvn_se_sess->se_node_acl,
			tv_nexus->tvn_se_sess, tv_nexus);
	tpg->tpg_nexus = tv_nexus;

	mutex_unlock(&tpg->tv_tpg_mutex);
	return 0;

out:
	vhost_scsi_free_cmd_map_res(tv_nexus, se_sess);
	transport_free_session(se_sess);
	kfree(tv_nexus);
	return -ENOMEM;
}
