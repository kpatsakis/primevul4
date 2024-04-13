int security_transition_sid_user(u32 ssid, u32 tsid, u16 tclass,
				 const char *objname, u32 *out_sid)
{
	return security_compute_sid(ssid, tsid, tclass, AVTAB_TRANSITION,
				    objname, out_sid, false);
}
