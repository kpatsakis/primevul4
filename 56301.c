oui_to_struct_tok(uint32_t orgcode)
{
	const struct tok *tok = null_values;
	const struct oui_tok *otp;

	for (otp = &oui_to_tok[0]; otp->tok != NULL; otp++) {
		if (otp->oui == orgcode) {
			tok = otp->tok;
			break;
		}
	}
	return (tok);
}
