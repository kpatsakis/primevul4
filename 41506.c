static inline u32 tg3_set_function_status(struct tg3 *tp, u32 newstat)
{
	u32 status, shift;

	if (tg3_asic_rev(tp) == ASIC_REV_5717 ||
	    tg3_asic_rev(tp) == ASIC_REV_5719)
		status = tg3_ape_read32(tp, TG3_APE_GPIO_MSG);
	else
		status = tr32(TG3_CPMU_DRV_STATUS);

	shift = TG3_APE_GPIO_MSG_SHIFT + 4 * tp->pci_fn;
	status &= ~(TG3_GPIO_MSG_MASK << shift);
	status |= (newstat << shift);

	if (tg3_asic_rev(tp) == ASIC_REV_5717 ||
	    tg3_asic_rev(tp) == ASIC_REV_5719)
		tg3_ape_write32(tp, TG3_APE_GPIO_MSG, status);
	else
		tw32(TG3_CPMU_DRV_STATUS, status);

	return status >> TG3_APE_GPIO_MSG_SHIFT;
}
