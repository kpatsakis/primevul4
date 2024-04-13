static void remove_vqs(struct ports_device *portdev)
{
	portdev->vdev->config->del_vqs(portdev->vdev);
	kfree(portdev->in_vqs);
	kfree(portdev->out_vqs);
}
