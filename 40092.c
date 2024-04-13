static int adjust_tp(struct atm_trafprm *tp, unsigned char aal)
{
	int max_sdu;

	if (!tp->traffic_class)
		return 0;
	switch (aal) {
	case ATM_AAL0:
		max_sdu = ATM_CELL_SIZE-1;
		break;
	case ATM_AAL34:
		max_sdu = ATM_MAX_AAL34_PDU;
		break;
	default:
		pr_warning("AAL problems ... (%d)\n", aal);
		/* fall through */
	case ATM_AAL5:
		max_sdu = ATM_MAX_AAL5_PDU;
	}
	if (!tp->max_sdu)
		tp->max_sdu = max_sdu;
	else if (tp->max_sdu > max_sdu)
		return -EINVAL;
	if (!tp->max_cdv)
		tp->max_cdv = ATM_MAX_CDV;
	return 0;
}
