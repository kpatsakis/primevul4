static int vop_virtio_config_change(struct vop_vdev *vdev, void *argp)
{
	DECLARE_WAIT_QUEUE_HEAD_ONSTACK(wake);
	int ret = 0, retry, i;
	struct vop_device *vpdev = vdev->vpdev;
	struct vop_info *vi = dev_get_drvdata(&vpdev->dev);
	struct mic_bootparam *bootparam = vpdev->hw_ops->get_dp(vpdev);
	s8 db = bootparam->h2c_config_db;

	mutex_lock(&vi->vop_mutex);
	for (i = 0; i < vdev->dd->num_vq; i++)
		mutex_lock_nested(&vdev->vvr[i].vr_mutex, i + 1);

	if (db == -1 || vdev->dd->type == -1) {
		ret = -EIO;
		goto exit;
	}

	memcpy(mic_vq_configspace(vdev->dd), argp, vdev->dd->config_len);
	vdev->dc->config_change = MIC_VIRTIO_PARAM_CONFIG_CHANGED;
	vpdev->hw_ops->send_intr(vpdev, db);

	for (retry = 100; retry--;) {
		ret = wait_event_timeout(wake, vdev->dc->guest_ack,
					 msecs_to_jiffies(100));
		if (ret)
			break;
	}

	dev_dbg(vop_dev(vdev),
		"%s %d retry: %d\n", __func__, __LINE__, retry);
	vdev->dc->config_change = 0;
	vdev->dc->guest_ack = 0;
exit:
	for (i = 0; i < vdev->dd->num_vq; i++)
		mutex_unlock(&vdev->vvr[i].vr_mutex);
	mutex_unlock(&vi->vop_mutex);
	return ret;
}
