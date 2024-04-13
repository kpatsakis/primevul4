static int __vcc_connect(struct atm_vcc *vcc, struct atm_dev *dev, short vpi,
			 int vci)
{
	struct sock *sk = sk_atm(vcc);
	int error;

	if ((vpi != ATM_VPI_UNSPEC && vpi != ATM_VPI_ANY &&
	    vpi >> dev->ci_range.vpi_bits) || (vci != ATM_VCI_UNSPEC &&
	    vci != ATM_VCI_ANY && vci >> dev->ci_range.vci_bits))
		return -EINVAL;
	if (vci > 0 && vci < ATM_NOT_RSV_VCI && !capable(CAP_NET_BIND_SERVICE))
		return -EPERM;
	error = -ENODEV;
	if (!try_module_get(dev->ops->owner))
		return error;
	vcc->dev = dev;
	write_lock_irq(&vcc_sklist_lock);
	if (test_bit(ATM_DF_REMOVED, &dev->flags) ||
	    (error = find_ci(vcc, &vpi, &vci))) {
		write_unlock_irq(&vcc_sklist_lock);
		goto fail_module_put;
	}
	vcc->vpi = vpi;
	vcc->vci = vci;
	__vcc_insert_socket(sk);
	write_unlock_irq(&vcc_sklist_lock);
	switch (vcc->qos.aal) {
	case ATM_AAL0:
		error = atm_init_aal0(vcc);
		vcc->stats = &dev->stats.aal0;
		break;
	case ATM_AAL34:
		error = atm_init_aal34(vcc);
		vcc->stats = &dev->stats.aal34;
		break;
	case ATM_NO_AAL:
		/* ATM_AAL5 is also used in the "0 for default" case */
		vcc->qos.aal = ATM_AAL5;
		/* fall through */
	case ATM_AAL5:
		error = atm_init_aal5(vcc);
		vcc->stats = &dev->stats.aal5;
		break;
	default:
		error = -EPROTOTYPE;
	}
	if (!error)
		error = adjust_tp(&vcc->qos.txtp, vcc->qos.aal);
	if (!error)
		error = adjust_tp(&vcc->qos.rxtp, vcc->qos.aal);
	if (error)
		goto fail;
	pr_debug("VCC %d.%d, AAL %d\n", vpi, vci, vcc->qos.aal);
	pr_debug("  TX: %d, PCR %d..%d, SDU %d\n",
		 vcc->qos.txtp.traffic_class,
		 vcc->qos.txtp.min_pcr,
		 vcc->qos.txtp.max_pcr,
		 vcc->qos.txtp.max_sdu);
	pr_debug("  RX: %d, PCR %d..%d, SDU %d\n",
		 vcc->qos.rxtp.traffic_class,
		 vcc->qos.rxtp.min_pcr,
		 vcc->qos.rxtp.max_pcr,
		 vcc->qos.rxtp.max_sdu);

	if (dev->ops->open) {
		error = dev->ops->open(vcc);
		if (error)
			goto fail;
	}
	return 0;

fail:
	vcc_remove_socket(sk);
fail_module_put:
	module_put(dev->ops->owner);
	/* ensure we get dev module ref count correct */
	vcc->dev = NULL;
	return error;
}
