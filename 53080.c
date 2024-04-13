static void atl2_poll_controller(struct net_device *netdev)
{
	disable_irq(netdev->irq);
	atl2_intr(netdev->irq, netdev);
	enable_irq(netdev->irq);
}
