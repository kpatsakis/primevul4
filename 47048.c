static bool virtnet_fail_on_feature(struct virtio_device *vdev,
				    unsigned int fbit,
				    const char *fname, const char *dname)
{
	if (!virtio_has_feature(vdev, fbit))
		return false;

	dev_err(&vdev->dev, "device advertises feature %s but not %s",
		fname, dname);

	return true;
}
