static int check_ci(const struct atm_vcc *vcc, short vpi, int vci)
{
	struct hlist_head *head = &vcc_hash[vci & (VCC_HTABLE_SIZE - 1)];
	struct sock *s;
	struct atm_vcc *walk;

	sk_for_each(s, head) {
		walk = atm_sk(s);
		if (walk->dev != vcc->dev)
			continue;
		if (test_bit(ATM_VF_ADDR, &walk->flags) && walk->vpi == vpi &&
		    walk->vci == vci && ((walk->qos.txtp.traffic_class !=
		    ATM_NONE && vcc->qos.txtp.traffic_class != ATM_NONE) ||
		    (walk->qos.rxtp.traffic_class != ATM_NONE &&
		    vcc->qos.rxtp.traffic_class != ATM_NONE)))
			return -EADDRINUSE;
	}

	/* allow VCCs with same VPI/VCI iff they don't collide on
	   TX/RX (but we may refuse such sharing for other reasons,
	   e.g. if protocol requires to have both channels) */

	return 0;
}
