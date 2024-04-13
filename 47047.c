static void virtnet_del_vqs(struct virtnet_info *vi)
{
	struct virtio_device *vdev = vi->vdev;

	virtnet_clean_affinity(vi, -1);

	vdev->config->del_vqs(vdev);

	virtnet_free_queues(vi);
}
