int ppp_register_channel(struct ppp_channel *chan)
{
	return ppp_register_net_channel(current->nsproxy->net_ns, chan);
}
