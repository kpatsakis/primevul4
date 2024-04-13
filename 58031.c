static void cp2112_gpio_irq_unmask(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct cp2112_device *dev = gpiochip_get_data(gc);

	__set_bit(d->hwirq, &dev->irq_mask);
}
