static void tg3_ints_fini(struct tg3 *tp)
{
	if (tg3_flag(tp, USING_MSIX))
		pci_disable_msix(tp->pdev);
	else if (tg3_flag(tp, USING_MSI))
		pci_disable_msi(tp->pdev);
	tg3_flag_clear(tp, USING_MSI);
	tg3_flag_clear(tp, USING_MSIX);
	tg3_flag_clear(tp, ENABLE_RSS);
	tg3_flag_clear(tp, ENABLE_TSS);
}
