static void vop_dev_remove(struct vop_info *pvi, struct mic_device_ctrl *devp,
			   struct vop_device *vpdev)
{
	struct mic_bootparam *bootparam = vpdev->hw_ops->get_dp(vpdev);
	s8 db;
	int ret, retry;
	DECLARE_WAIT_QUEUE_HEAD_ONSTACK(wake);

	devp->config_change = MIC_VIRTIO_PARAM_DEV_REMOVE;
	db = bootparam->h2c_config_db;
	if (db != -1)
		vpdev->hw_ops->send_intr(vpdev, db);
	else
		goto done;
	for (retry = 15; retry--;) {
		ret = wait_event_timeout(wake, devp->guest_ack,
					 msecs_to_jiffies(1000));
		if (ret)
			break;
	}
done:
	devp->config_change = 0;
	devp->guest_ack = 0;
}
