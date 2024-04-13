static int vhost_scsi_register_configfs(void)
{
	struct target_fabric_configfs *fabric;
	int ret;

	pr_debug("vhost-scsi fabric module %s on %s/%s"
		" on "UTS_RELEASE"\n", VHOST_SCSI_VERSION, utsname()->sysname,
		utsname()->machine);
	/*
	 * Register the top level struct config_item_type with TCM core
	 */
	fabric = target_fabric_configfs_init(THIS_MODULE, "vhost");
	if (IS_ERR(fabric)) {
		pr_err("target_fabric_configfs_init() failed\n");
		return PTR_ERR(fabric);
	}
	/*
	 * Setup fabric->tf_ops from our local vhost_scsi_ops
	 */
	fabric->tf_ops = vhost_scsi_ops;
	/*
	 * Setup default attribute lists for various fabric->tf_cit_tmpl
	 */
	fabric->tf_cit_tmpl.tfc_wwn_cit.ct_attrs = vhost_scsi_wwn_attrs;
	fabric->tf_cit_tmpl.tfc_tpg_base_cit.ct_attrs = vhost_scsi_tpg_attrs;
	fabric->tf_cit_tmpl.tfc_tpg_attrib_cit.ct_attrs = NULL;
	fabric->tf_cit_tmpl.tfc_tpg_param_cit.ct_attrs = NULL;
	fabric->tf_cit_tmpl.tfc_tpg_np_base_cit.ct_attrs = NULL;
	fabric->tf_cit_tmpl.tfc_tpg_nacl_base_cit.ct_attrs = NULL;
	fabric->tf_cit_tmpl.tfc_tpg_nacl_attrib_cit.ct_attrs = NULL;
	fabric->tf_cit_tmpl.tfc_tpg_nacl_auth_cit.ct_attrs = NULL;
	fabric->tf_cit_tmpl.tfc_tpg_nacl_param_cit.ct_attrs = NULL;
	/*
	 * Register the fabric for use within TCM
	 */
	ret = target_fabric_configfs_register(fabric);
	if (ret < 0) {
		pr_err("target_fabric_configfs_register() failed"
				" for TCM_VHOST\n");
		return ret;
	}
	/*
	 * Setup our local pointer to *fabric
	 */
	vhost_scsi_fabric_configfs = fabric;
	pr_debug("TCM_VHOST[0] - Set fabric -> vhost_scsi_fabric_configfs\n");
	return 0;
};
