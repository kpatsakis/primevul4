static void cp2112_gpio_poll_callback(struct work_struct *work)
{
	struct cp2112_device *dev = container_of(work, struct cp2112_device,
						 gpio_poll_worker.work);
	struct irq_data *d;
	u8 gpio_mask;
	u8 virqs = (u8)dev->irq_mask;
	u32 irq_type;
	int irq, virq, ret;

	ret = cp2112_gpio_get_all(&dev->gc);
	if (ret == -ENODEV) /* the hardware has been disconnected */
		return;
	if (ret < 0)
		goto exit;

	gpio_mask = ret;

	while (virqs) {
		virq = ffs(virqs) - 1;
		virqs &= ~BIT(virq);

		if (!dev->gc.to_irq)
			break;

		irq = dev->gc.to_irq(&dev->gc, virq);

		d = irq_get_irq_data(irq);
		if (!d)
			continue;

		irq_type = irqd_get_trigger_type(d);

		if (gpio_mask & BIT(virq)) {
			/* Level High */

			if (irq_type & IRQ_TYPE_LEVEL_HIGH)
				handle_nested_irq(irq);

			if ((irq_type & IRQ_TYPE_EDGE_RISING) &&
			    !(dev->gpio_prev_state & BIT(virq)))
				handle_nested_irq(irq);
		} else {
			/* Level Low */

			if (irq_type & IRQ_TYPE_LEVEL_LOW)
				handle_nested_irq(irq);

			if ((irq_type & IRQ_TYPE_EDGE_FALLING) &&
			    (dev->gpio_prev_state & BIT(virq)))
				handle_nested_irq(irq);
		}
	}

	dev->gpio_prev_state = gpio_mask;

exit:
	if (dev->gpio_poll)
		schedule_delayed_work(&dev->gpio_poll_worker, 10);
}
