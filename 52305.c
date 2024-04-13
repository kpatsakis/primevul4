static void usbip_dump_usb_device(struct usb_device *udev)
{
	struct device *dev = &udev->dev;
	int i;

	dev_dbg(dev, "       devnum(%d) devpath(%s) usb speed(%s)",
		udev->devnum, udev->devpath, usb_speed_string(udev->speed));

	pr_debug("tt %p, ttport %d\n", udev->tt, udev->ttport);

	dev_dbg(dev, "                    ");
	for (i = 0; i < 16; i++)
		pr_debug(" %2u", i);
	pr_debug("\n");

	dev_dbg(dev, "       toggle0(IN) :");
	for (i = 0; i < 16; i++)
		pr_debug(" %2u", (udev->toggle[0] & (1 << i)) ? 1 : 0);
	pr_debug("\n");

	dev_dbg(dev, "       toggle1(OUT):");
	for (i = 0; i < 16; i++)
		pr_debug(" %2u", (udev->toggle[1] & (1 << i)) ? 1 : 0);
	pr_debug("\n");

	dev_dbg(dev, "       epmaxp_in   :");
	for (i = 0; i < 16; i++) {
		if (udev->ep_in[i])
			pr_debug(" %2u",
			    le16_to_cpu(udev->ep_in[i]->desc.wMaxPacketSize));
	}
	pr_debug("\n");

	dev_dbg(dev, "       epmaxp_out  :");
	for (i = 0; i < 16; i++) {
		if (udev->ep_out[i])
			pr_debug(" %2u",
			    le16_to_cpu(udev->ep_out[i]->desc.wMaxPacketSize));
	}
	pr_debug("\n");

	dev_dbg(dev, "parent %p, bus %p\n", udev->parent, udev->bus);

	dev_dbg(dev,
		"descriptor %p, config %p, actconfig %p, rawdescriptors %p\n",
		&udev->descriptor, udev->config,
		udev->actconfig, udev->rawdescriptors);

	dev_dbg(dev, "have_langid %d, string_langid %d\n",
		udev->have_langid, udev->string_langid);

	dev_dbg(dev, "maxchild %d\n", udev->maxchild);
}
