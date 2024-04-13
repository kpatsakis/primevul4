R_API RConfigHold* r_config_hold_new(RConfig *cfg) {
	if (cfg) {
		RConfigHold *hold = R_NEW0 (RConfigHold);
		if (hold) {
			hold->cfg = cfg;
			return hold;
		}
	}
	return NULL;
}
