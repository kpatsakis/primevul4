static int __maybe_unused cp2112_allocate_irq(struct cp2112_device *dev,
					      int pin)
{
	int ret;

	if (dev->desc[pin])
		return -EINVAL;

	dev->desc[pin] = gpiochip_request_own_desc(&dev->gc, pin,
						   "HID/I2C:Event");
	if (IS_ERR(dev->desc[pin])) {
		dev_err(dev->gc.parent, "Failed to request GPIO\n");
		return PTR_ERR(dev->desc[pin]);
	}

	ret = gpiochip_lock_as_irq(&dev->gc, pin);
	if (ret) {
		dev_err(dev->gc.parent, "Failed to lock GPIO as interrupt\n");
		goto err_desc;
	}

	ret = gpiod_to_irq(dev->desc[pin]);
	if (ret < 0) {
		dev_err(dev->gc.parent, "Failed to translate GPIO to IRQ\n");
		goto err_lock;
	}

	return ret;

err_lock:
	gpiochip_unlock_as_irq(&dev->gc, pin);
err_desc:
	gpiochip_free_own_desc(dev->desc[pin]);
	dev->desc[pin] = NULL;
	return ret;
}
