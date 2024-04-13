static void virtnet_get_drvinfo(struct net_device *dev,
				struct ethtool_drvinfo *info)
{
	struct virtnet_info *vi = netdev_priv(dev);
	struct virtio_device *vdev = vi->vdev;

	strlcpy(info->driver, KBUILD_MODNAME, sizeof(info->driver));
	strlcpy(info->version, VIRTNET_DRIVER_VERSION, sizeof(info->version));
	strlcpy(info->bus_info, virtio_bus_name(vdev), sizeof(info->bus_info));

}
