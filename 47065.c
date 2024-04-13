static void virtnet_set_rx_mode(struct net_device *dev)
{
	struct virtnet_info *vi = netdev_priv(dev);
	struct scatterlist sg[2];
	u8 promisc, allmulti;
	struct virtio_net_ctrl_mac *mac_data;
	struct netdev_hw_addr *ha;
	int uc_count;
	int mc_count;
	void *buf;
	int i;

	/* We can't dynamically set ndo_set_rx_mode, so return gracefully */
	if (!virtio_has_feature(vi->vdev, VIRTIO_NET_F_CTRL_RX))
		return;

	promisc = ((dev->flags & IFF_PROMISC) != 0);
	allmulti = ((dev->flags & IFF_ALLMULTI) != 0);

	sg_init_one(sg, &promisc, sizeof(promisc));

	if (!virtnet_send_command(vi, VIRTIO_NET_CTRL_RX,
				  VIRTIO_NET_CTRL_RX_PROMISC, sg))
		dev_warn(&dev->dev, "Failed to %sable promisc mode.\n",
			 promisc ? "en" : "dis");

	sg_init_one(sg, &allmulti, sizeof(allmulti));

	if (!virtnet_send_command(vi, VIRTIO_NET_CTRL_RX,
				  VIRTIO_NET_CTRL_RX_ALLMULTI, sg))
		dev_warn(&dev->dev, "Failed to %sable allmulti mode.\n",
			 allmulti ? "en" : "dis");

	uc_count = netdev_uc_count(dev);
	mc_count = netdev_mc_count(dev);
	/* MAC filter - use one buffer for both lists */
	buf = kzalloc(((uc_count + mc_count) * ETH_ALEN) +
		      (2 * sizeof(mac_data->entries)), GFP_ATOMIC);
	mac_data = buf;
	if (!buf)
		return;

	sg_init_table(sg, 2);

	/* Store the unicast list and count in the front of the buffer */
	mac_data->entries = cpu_to_virtio32(vi->vdev, uc_count);
	i = 0;
	netdev_for_each_uc_addr(ha, dev)
		memcpy(&mac_data->macs[i++][0], ha->addr, ETH_ALEN);

	sg_set_buf(&sg[0], mac_data,
		   sizeof(mac_data->entries) + (uc_count * ETH_ALEN));

	/* multicast list and count fill the end */
	mac_data = (void *)&mac_data->macs[uc_count][0];

	mac_data->entries = cpu_to_virtio32(vi->vdev, mc_count);
	i = 0;
	netdev_for_each_mc_addr(ha, dev)
		memcpy(&mac_data->macs[i++][0], ha->addr, ETH_ALEN);

	sg_set_buf(&sg[1], mac_data,
		   sizeof(mac_data->entries) + (mc_count * ETH_ALEN));

	if (!virtnet_send_command(vi, VIRTIO_NET_CTRL_MAC,
				  VIRTIO_NET_CTRL_MAC_TABLE_SET, sg))
		dev_warn(&dev->dev, "Failed to set MAC filter table.\n");

	kfree(buf);
}
