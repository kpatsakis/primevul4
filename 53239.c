static void process_one_v4_ace(struct posix_acl_state *state,
				struct nfs4_ace *ace)
{
	u32 mask = ace->access_mask;
	int i;

	state->empty = 0;

	switch (ace2type(ace)) {
	case ACL_USER_OBJ:
		if (ace->type == NFS4_ACE_ACCESS_ALLOWED_ACE_TYPE) {
			allow_bits(&state->owner, mask);
		} else {
			deny_bits(&state->owner, mask);
		}
		break;
	case ACL_USER:
		i = find_uid(state, ace->who_uid);
		if (ace->type == NFS4_ACE_ACCESS_ALLOWED_ACE_TYPE) {
			allow_bits(&state->users->aces[i].perms, mask);
		} else {
			deny_bits(&state->users->aces[i].perms, mask);
			mask = state->users->aces[i].perms.deny;
			deny_bits(&state->owner, mask);
		}
		break;
	case ACL_GROUP_OBJ:
		if (ace->type == NFS4_ACE_ACCESS_ALLOWED_ACE_TYPE) {
			allow_bits(&state->group, mask);
		} else {
			deny_bits(&state->group, mask);
			mask = state->group.deny;
			deny_bits(&state->owner, mask);
			deny_bits(&state->everyone, mask);
			deny_bits_array(state->users, mask);
			deny_bits_array(state->groups, mask);
		}
		break;
	case ACL_GROUP:
		i = find_gid(state, ace->who_gid);
		if (ace->type == NFS4_ACE_ACCESS_ALLOWED_ACE_TYPE) {
			allow_bits(&state->groups->aces[i].perms, mask);
		} else {
			deny_bits(&state->groups->aces[i].perms, mask);
			mask = state->groups->aces[i].perms.deny;
			deny_bits(&state->owner, mask);
			deny_bits(&state->group, mask);
			deny_bits(&state->everyone, mask);
			deny_bits_array(state->users, mask);
			deny_bits_array(state->groups, mask);
		}
		break;
	case ACL_OTHER:
		if (ace->type == NFS4_ACE_ACCESS_ALLOWED_ACE_TYPE) {
			allow_bits(&state->owner, mask);
			allow_bits(&state->group, mask);
			allow_bits(&state->other, mask);
			allow_bits(&state->everyone, mask);
			allow_bits_array(state->users, mask);
			allow_bits_array(state->groups, mask);
		} else {
			deny_bits(&state->owner, mask);
			deny_bits(&state->group, mask);
			deny_bits(&state->other, mask);
			deny_bits(&state->everyone, mask);
			deny_bits_array(state->users, mask);
			deny_bits_array(state->groups, mask);
		}
	}
}
