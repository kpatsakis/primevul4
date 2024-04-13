static unsigned int *generic_get_timeouts(struct net *net)
{
	return &(generic_pernet(net)->timeout);
}
