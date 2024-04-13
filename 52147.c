iperf_set_send_state(struct iperf_test *test, signed char state)
{
    test->state = state;
    if (Nwrite(test->ctrl_sck, (char*) &state, sizeof(state), Ptcp) < 0) {
	i_errno = IESENDMESSAGE;
	return -1;
    }
    return 0;
}
