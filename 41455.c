static void tg3_power_down(struct tg3 *tp)
{
	tg3_power_down_prepare(tp);

	pci_wake_from_d3(tp->pdev, tg3_flag(tp, WOL_ENABLE));
	pci_set_power_state(tp->pdev, PCI_D3hot);
}
