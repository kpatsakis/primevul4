static void cp2112_gpio_irq_mask(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct cp2112_device *dev = gpiochip_get_data(gc);

	__clear_bit(d->hwirq, &dev->irq_mask);
}
