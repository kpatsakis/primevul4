void opj_pi_destroy(opj_pi_iterator_t *p_pi,
                    OPJ_UINT32 p_nb_elements)
{
	OPJ_UINT32 compno, pino;
	opj_pi_iterator_t *l_current_pi = p_pi;
    if (p_pi) {
		if (p_pi->include) {
			opj_free(p_pi->include);
			p_pi->include = 00;
		}
		for (pino = 0; pino < p_nb_elements; ++pino){
			if(l_current_pi->comps) {
				opj_pi_comp_t *l_current_component = l_current_pi->comps;
                for (compno = 0; compno < l_current_pi->numcomps; compno++){
                    if(l_current_component->resolutions) {
						opj_free(l_current_component->resolutions);
						l_current_component->resolutions = 00;
					}

					++l_current_component;
				}
				opj_free(l_current_pi->comps);
				l_current_pi->comps = 0;
			}
			++l_current_pi;
		}
		opj_free(p_pi);
	}
}
