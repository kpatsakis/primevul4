static opj_pi_iterator_t * opj_pi_create(	const opj_image_t *image,
                                    const opj_cp_t *cp,
                                    OPJ_UINT32 tileno )
{
	/* loop*/
	OPJ_UINT32 pino, compno;
	/* number of poc in the p_pi*/
	OPJ_UINT32 l_poc_bound;

	/* pointers to tile coding parameters and components.*/
	opj_pi_iterator_t *l_pi = 00;
	opj_tcp_t *tcp = 00;
	const opj_tccp_t *tccp = 00;

	/* current packet iterator being allocated*/
	opj_pi_iterator_t *l_current_pi = 00;

	/* preconditions in debug*/
	assert(cp != 00);
	assert(image != 00);
	assert(tileno < cp->tw * cp->th);

	/* initializations*/
	tcp = &cp->tcps[tileno];
	l_poc_bound = tcp->numpocs+1;

	/* memory allocations*/
	l_pi = (opj_pi_iterator_t*) opj_calloc((l_poc_bound), sizeof(opj_pi_iterator_t));
	if (!l_pi) {
		return NULL;
	}

	l_current_pi = l_pi;
	for (pino = 0; pino < l_poc_bound ; ++pino) {

		l_current_pi->comps = (opj_pi_comp_t*) opj_calloc(image->numcomps, sizeof(opj_pi_comp_t));
		if (! l_current_pi->comps) {
			opj_pi_destroy(l_pi, l_poc_bound);
			return NULL;
		}

		l_current_pi->numcomps = image->numcomps;

		for (compno = 0; compno < image->numcomps; ++compno) {
			opj_pi_comp_t *comp = &l_current_pi->comps[compno];

			tccp = &tcp->tccps[compno];

			comp->resolutions = (opj_pi_resolution_t*) opj_calloc(tccp->numresolutions, sizeof(opj_pi_resolution_t));
			if (!comp->resolutions) {
				opj_pi_destroy(l_pi, l_poc_bound);
				return 00;
			}

			comp->numresolutions = tccp->numresolutions;
		}
		++l_current_pi;
	}
	return l_pi;
}
