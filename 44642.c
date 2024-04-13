int security_transition_sid(u32 ssid, u32 tsid, u16 tclass,
			    const struct qstr *qstr, u32 *out_sid)
{
	return security_compute_sid(ssid, tsid, tclass, AVTAB_TRANSITION,
				    qstr ? qstr->name : NULL, out_sid, true);
}
