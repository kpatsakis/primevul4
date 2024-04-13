int vcc_setsockopt(struct socket *sock, int level, int optname,
		   char __user *optval, unsigned int optlen)
{
	struct atm_vcc *vcc;
	unsigned long value;
	int error;

	if (__SO_LEVEL_MATCH(optname, level) && optlen != __SO_SIZE(optname))
		return -EINVAL;

	vcc = ATM_SD(sock);
	switch (optname) {
	case SO_ATMQOS:
	{
		struct atm_qos qos;

		if (copy_from_user(&qos, optval, sizeof(qos)))
			return -EFAULT;
		error = check_qos(&qos);
		if (error)
			return error;
		if (sock->state == SS_CONNECTED)
			return atm_change_qos(vcc, &qos);
		if (sock->state != SS_UNCONNECTED)
			return -EBADFD;
		vcc->qos = qos;
		set_bit(ATM_VF_HASQOS, &vcc->flags);
		return 0;
	}
	case SO_SETCLP:
		if (get_user(value, (unsigned long __user *)optval))
			return -EFAULT;
		if (value)
			vcc->atm_options |= ATM_ATMOPT_CLP;
		else
			vcc->atm_options &= ~ATM_ATMOPT_CLP;
		return 0;
	default:
		if (level == SOL_SOCKET)
			return -EINVAL;
		break;
	}
	if (!vcc->dev || !vcc->dev->ops->setsockopt)
		return -EINVAL;
	return vcc->dev->ops->setsockopt(vcc, level, optname, optval, optlen);
}
