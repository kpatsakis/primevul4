static int atm_change_qos(struct atm_vcc *vcc, struct atm_qos *qos)
{
	int error;

	/*
	 * Don't let the QoS change the already connected AAL type nor the
	 * traffic class.
	 */
	if (qos->aal != vcc->qos.aal ||
	    qos->rxtp.traffic_class != vcc->qos.rxtp.traffic_class ||
	    qos->txtp.traffic_class != vcc->qos.txtp.traffic_class)
		return -EINVAL;
	error = adjust_tp(&qos->txtp, qos->aal);
	if (!error)
		error = adjust_tp(&qos->rxtp, qos->aal);
	if (error)
		return error;
	if (!vcc->dev->ops->change_qos)
		return -EOPNOTSUPP;
	if (sk_atm(vcc)->sk_family == AF_ATMPVC)
		return vcc->dev->ops->change_qos(vcc, qos, ATM_MF_SET);
	return svc_change_qos(vcc, qos);
}
