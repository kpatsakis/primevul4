static void gs_destroy_candev(struct gs_can *dev)
{
	unregister_candev(dev->netdev);
	usb_kill_anchored_urbs(&dev->tx_submitted);
	free_candev(dev->netdev);
}
