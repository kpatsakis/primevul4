static int vop_copy_dp_entry(struct vop_vdev *vdev,
			     struct mic_device_desc *argp, __u8 *type,
			     struct mic_device_desc **devpage)
{
	struct vop_device *vpdev = vdev->vpdev;
	struct mic_device_desc *devp;
	struct mic_vqconfig *vqconfig;
	int ret = 0, i;
	bool slot_found = false;

	vqconfig = mic_vq_config(argp);
	for (i = 0; i < argp->num_vq; i++) {
		if (le16_to_cpu(vqconfig[i].num) > MIC_MAX_VRING_ENTRIES) {
			ret =  -EINVAL;
			dev_err(vop_dev(vdev), "%s %d err %d\n",
				__func__, __LINE__, ret);
			goto exit;
		}
	}

	/* Find the first free device page entry */
	for (i = sizeof(struct mic_bootparam);
		i < MIC_DP_SIZE - mic_total_desc_size(argp);
		i += mic_total_desc_size(devp)) {
		devp = vpdev->hw_ops->get_dp(vpdev) + i;
		if (devp->type == 0 || devp->type == -1) {
			slot_found = true;
			break;
		}
	}
	if (!slot_found) {
		ret =  -EINVAL;
		dev_err(vop_dev(vdev), "%s %d err %d\n",
			__func__, __LINE__, ret);
		goto exit;
	}
	/*
	 * Save off the type before doing the memcpy. Type will be set in the
	 * end after completing all initialization for the new device.
	 */
	*type = argp->type;
	argp->type = 0;
	memcpy(devp, argp, mic_desc_size(argp));

	*devpage = devp;
exit:
	return ret;
}
