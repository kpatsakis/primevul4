static void skb_xmit_done(struct virtqueue *vq)
{
	struct virtnet_info *vi = vq->vdev->priv;

	/* Suppress further interrupts. */
	virtqueue_disable_cb(vq);

	/* We were probably waiting for more output buffers. */
	netif_wake_subqueue(vi->dev, vq2txq(vq));
}
