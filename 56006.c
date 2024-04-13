static void set_lwt_redirect(struct rtable *rth)
{
	if (lwtunnel_output_redirect(rth->dst.lwtstate)) {
		rth->dst.lwtstate->orig_output = rth->dst.output;
		rth->dst.output = lwtunnel_output;
	}

	if (lwtunnel_input_redirect(rth->dst.lwtstate)) {
		rth->dst.lwtstate->orig_input = rth->dst.input;
		rth->dst.input = lwtunnel_input;
	}
}
