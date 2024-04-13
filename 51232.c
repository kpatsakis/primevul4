static void opj_pi_update_decode_not_poc (opj_pi_iterator_t * p_pi,
                                   opj_tcp_t * p_tcp,
                                   OPJ_UINT32 p_max_precision,
                                   OPJ_UINT32 p_max_res)
{
	/* loop*/
	OPJ_UINT32 pino;

	/* encoding prameters to set*/
	OPJ_UINT32 l_bound;

	opj_pi_iterator_t * l_current_pi = 00;
	/* preconditions in debug*/
	assert(p_tcp != 00);
	assert(p_pi != 00);

	/* initializations*/
	l_bound = p_tcp->numpocs+1;
	l_current_pi = p_pi;

	for (pino = 0;pino<l_bound;++pino) {
		l_current_pi->poc.prg = p_tcp->prg;
		l_current_pi->first = 1;
		l_current_pi->poc.resno0 = 0;
		l_current_pi->poc.compno0 = 0;
		l_current_pi->poc.layno0 = 0;
		l_current_pi->poc.precno0 = 0;
		l_current_pi->poc.resno1 = p_max_res;
		l_current_pi->poc.compno1 = l_current_pi->numcomps;
		l_current_pi->poc.layno1 = p_tcp->numlayers;
		l_current_pi->poc.precno1 = p_max_precision;
		++l_current_pi;
	}
}
