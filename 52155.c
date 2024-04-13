iperf_set_test_role(struct iperf_test *ipt, char role)
{
    ipt->role = role;
    if (role == 'c')
	ipt->sender = 1;
    else if (role == 's')
	ipt->sender = 0;
    if (ipt->reverse)
        ipt->sender = ! ipt->sender;
    check_sender_has_retransmits(ipt);
}
