int vcc_connect(struct socket *sock, int itf, short vpi, int vci)
{
	struct atm_dev *dev;
	struct atm_vcc *vcc = ATM_SD(sock);
	int error;

	pr_debug("(vpi %d, vci %d)\n", vpi, vci);
	if (sock->state == SS_CONNECTED)
		return -EISCONN;
	if (sock->state != SS_UNCONNECTED)
		return -EINVAL;
	if (!(vpi || vci))
		return -EINVAL;

	if (vpi != ATM_VPI_UNSPEC && vci != ATM_VCI_UNSPEC)
		clear_bit(ATM_VF_PARTIAL, &vcc->flags);
	else
		if (test_bit(ATM_VF_PARTIAL, &vcc->flags))
			return -EINVAL;
	pr_debug("(TX: cl %d,bw %d-%d,sdu %d; "
		 "RX: cl %d,bw %d-%d,sdu %d,AAL %s%d)\n",
		 vcc->qos.txtp.traffic_class, vcc->qos.txtp.min_pcr,
		 vcc->qos.txtp.max_pcr, vcc->qos.txtp.max_sdu,
		 vcc->qos.rxtp.traffic_class, vcc->qos.rxtp.min_pcr,
		 vcc->qos.rxtp.max_pcr, vcc->qos.rxtp.max_sdu,
		 vcc->qos.aal == ATM_AAL5 ? "" :
		 vcc->qos.aal == ATM_AAL0 ? "" : " ??? code ",
		 vcc->qos.aal == ATM_AAL0 ? 0 : vcc->qos.aal);
	if (!test_bit(ATM_VF_HASQOS, &vcc->flags))
		return -EBADFD;
	if (vcc->qos.txtp.traffic_class == ATM_ANYCLASS ||
	    vcc->qos.rxtp.traffic_class == ATM_ANYCLASS)
		return -EINVAL;
	if (likely(itf != ATM_ITF_ANY)) {
		dev = try_then_request_module(atm_dev_lookup(itf),
					      "atm-device-%d", itf);
	} else {
		dev = NULL;
		mutex_lock(&atm_dev_mutex);
		if (!list_empty(&atm_devs)) {
			dev = list_entry(atm_devs.next,
					 struct atm_dev, dev_list);
			atm_dev_hold(dev);
		}
		mutex_unlock(&atm_dev_mutex);
	}
	if (!dev)
		return -ENODEV;
	error = __vcc_connect(vcc, dev, vpi, vci);
	if (error) {
		atm_dev_put(dev);
		return error;
	}
	if (vpi == ATM_VPI_UNSPEC || vci == ATM_VCI_UNSPEC)
		set_bit(ATM_VF_PARTIAL, &vcc->flags);
	if (test_bit(ATM_VF_READY, &ATM_SD(sock)->flags))
		sock->state = SS_CONNECTED;
	return 0;
}
