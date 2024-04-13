static void opj_pi_update_decode_poc (opj_pi_iterator_t * p_pi,
                               opj_tcp_t * p_tcp,
                               OPJ_UINT32 p_max_precision,
                               OPJ_UINT32 p_max_res)
{
	/* loop*/
	OPJ_UINT32 pino;

	/* encoding prameters to set*/
	OPJ_UINT32 l_bound;

	opj_pi_iterator_t * l_current_pi = 00;
	opj_poc_t* l_current_poc = 0;

    OPJ_ARG_NOT_USED(p_max_res);

	/* preconditions in debug*/
	assert(p_pi != 00);
	assert(p_tcp != 00);

	/* initializations*/
	l_bound = p_tcp->numpocs+1;
	l_current_pi = p_pi;
	l_current_poc = p_tcp->pocs;

	for	(pino = 0;pino<l_bound;++pino) {
		l_current_pi->poc.prg = l_current_poc->prg; /* Progression Order #0 */
		l_current_pi->first = 1;

		l_current_pi->poc.resno0 = l_current_poc->resno0; /* Resolution Level Index #0 (Start) */
		l_current_pi->poc.compno0 = l_current_poc->compno0; /* Component Index #0 (Start) */
		l_current_pi->poc.layno0 = 0;
		l_current_pi->poc.precno0 = 0;
		l_current_pi->poc.resno1 = l_current_poc->resno1; /* Resolution Level Index #0 (End) */
		l_current_pi->poc.compno1 = l_current_poc->compno1; /* Component Index #0 (End) */
		l_current_pi->poc.layno1 = l_current_poc->layno1; /* Layer Index #0 (End) */
		l_current_pi->poc.precno1 = p_max_precision;
		++l_current_pi;
		++l_current_poc;
	}
}
