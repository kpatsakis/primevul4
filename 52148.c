iperf_set_test_bind_address(struct iperf_test *ipt, char *bind_address)
{
    ipt->bind_address = strdup(bind_address);
}
