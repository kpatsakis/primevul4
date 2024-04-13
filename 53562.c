static void hub_pm_barrier_for_all_ports(struct usb_hub *hub)
{
	int i;

	for (i = 0; i < hub->hdev->maxchild; ++i)
		pm_runtime_barrier(&hub->ports[i]->dev);
}
