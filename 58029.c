static void cp2112_gpio_irq_shutdown(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct cp2112_device *dev = gpiochip_get_data(gc);

	cancel_delayed_work_sync(&dev->gpio_poll_worker);
}
