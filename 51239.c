me_sasl(struct Client *client_p, struct Client *source_p,
	int parc, const char *parv[])
{
	struct Client *target_p, *agent_p;

	/* Let propagate if not addressed to us, or if broadcast.
	 * Only SASL agents can answer global requests.
	 */
	if(strncmp(parv[2], me.id, 3))
		return 0;

	if((target_p = find_id(parv[2])) == NULL)
		return 0;

	if((agent_p = find_id(parv[1])) == NULL)
		return 0;

	if(source_p != agent_p->servptr) /* WTF?! */
		return 0;

	/* We only accept messages from SASL agents; these must have umode +S
	 * (so the server must be listed in a service{} block).
	 */
	if(!IsService(agent_p))
		return 0;

	/* Reject if someone has already answered. */
	if(*target_p->localClient->sasl_agent && strncmp(parv[1], target_p->localClient->sasl_agent, IDLEN))
		return 0;
	else if(!*target_p->localClient->sasl_agent)
		rb_strlcpy(target_p->localClient->sasl_agent, parv[1], IDLEN);

	if(*parv[3] == 'C')
		sendto_one(target_p, "AUTHENTICATE %s", parv[4]);
	else if(*parv[3] == 'D')
	{
		if(*parv[4] == 'F')
			sendto_one(target_p, form_str(ERR_SASLFAIL), me.name, EmptyString(target_p->name) ? "*" : target_p->name);
		else if(*parv[4] == 'S') {
			sendto_one(target_p, form_str(RPL_SASLSUCCESS), me.name, EmptyString(target_p->name) ? "*" : target_p->name);
			target_p->localClient->sasl_complete = 1;
			ServerStats.is_ssuc++;
		}
		*target_p->localClient->sasl_agent = '\0'; /* Blank the stored agent so someone else can answer */
	}
	else if(*parv[3] == 'M')
		sendto_one(target_p, form_str(RPL_SASLMECHS), me.name, EmptyString(target_p->name) ? "*" : target_p->name, parv[4]);

	return 0;
}
