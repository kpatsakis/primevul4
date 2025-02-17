int asn1_ber_decoder(const struct asn1_decoder *decoder,
		     void *context,
		     const unsigned char *data,
		     size_t datalen)
{
	const unsigned char *machine = decoder->machine;
	const asn1_action_t *actions = decoder->actions;
	size_t machlen = decoder->machlen;
	enum asn1_opcode op;
	unsigned char tag = 0, csp = 0, jsp = 0, optag = 0, hdr = 0;
	const char *errmsg;
	size_t pc = 0, dp = 0, tdp = 0, len = 0;
	int ret;

	unsigned char flags = 0;
#define FLAG_INDEFINITE_LENGTH	0x01
#define FLAG_MATCHED		0x02
#define FLAG_LAST_MATCHED	0x04 /* Last tag matched */
#define FLAG_CONS		0x20 /* Corresponds to CONS bit in the opcode tag
				      * - ie. whether or not we are going to parse
				      *   a compound type.
				      */

#define NR_CONS_STACK 10
	unsigned short cons_dp_stack[NR_CONS_STACK];
	unsigned short cons_datalen_stack[NR_CONS_STACK];
	unsigned char cons_hdrlen_stack[NR_CONS_STACK];
#define NR_JUMP_STACK 10
	unsigned char jump_stack[NR_JUMP_STACK];

	if (datalen > 65535)
		return -EMSGSIZE;

next_op:
	pr_debug("next_op: pc=\e[32m%zu\e[m/%zu dp=\e[33m%zu\e[m/%zu C=%d J=%d\n",
		 pc, machlen, dp, datalen, csp, jsp);
	if (unlikely(pc >= machlen))
		goto machine_overrun_error;
	op = machine[pc];
	if (unlikely(pc + asn1_op_lengths[op] > machlen))
		goto machine_overrun_error;

	/* If this command is meant to match a tag, then do that before
	 * evaluating the command.
	 */
	if (op <= ASN1_OP__MATCHES_TAG) {
		unsigned char tmp;

		/* Skip conditional matches if possible */
		if ((op & ASN1_OP_MATCH__COND && flags & FLAG_MATCHED) ||
		    (op & ASN1_OP_MATCH__SKIP && dp == datalen)) {
			flags &= ~FLAG_LAST_MATCHED;
			pc += asn1_op_lengths[op];
			goto next_op;
		}

		flags = 0;
		hdr = 2;

		/* Extract a tag from the data */
		if (unlikely(dp >= datalen - 1))
			goto data_overrun_error;
		tag = data[dp++];
		if (unlikely((tag & 0x1f) == ASN1_LONG_TAG))
			goto long_tag_not_supported;

		if (op & ASN1_OP_MATCH__ANY) {
			pr_debug("- any %02x\n", tag);
		} else {
			/* Extract the tag from the machine
			 * - Either CONS or PRIM are permitted in the data if
			 *   CONS is not set in the op stream, otherwise CONS
			 *   is mandatory.
			 */
			optag = machine[pc + 1];
			flags |= optag & FLAG_CONS;

			/* Determine whether the tag matched */
			tmp = optag ^ tag;
			tmp &= ~(optag & ASN1_CONS_BIT);
			pr_debug("- match? %02x %02x %02x\n", tag, optag, tmp);
			if (tmp != 0) {
				/* All odd-numbered tags are MATCH_OR_SKIP. */
				if (op & ASN1_OP_MATCH__SKIP) {
					pc += asn1_op_lengths[op];
					dp--;
					goto next_op;
				}
				goto tag_mismatch;
			}
		}
		flags |= FLAG_MATCHED;

		len = data[dp++];
		if (len > 0x7f) {
			if (unlikely(len == ASN1_INDEFINITE_LENGTH)) {
				/* Indefinite length */
				if (unlikely(!(tag & ASN1_CONS_BIT)))
					goto indefinite_len_primitive;
				flags |= FLAG_INDEFINITE_LENGTH;
				if (unlikely(2 > datalen - dp))
					goto data_overrun_error;
			} else {
				int n = len - 0x80;
				if (unlikely(n > 2))
					goto length_too_long;
				if (unlikely(dp >= datalen - n))
					goto data_overrun_error;
				hdr += n;
				for (len = 0; n > 0; n--) {
					len <<= 8;
					len |= data[dp++];
				}
				if (unlikely(len > datalen - dp))
					goto data_overrun_error;
			}
		}

		if (flags & FLAG_CONS) {
			/* For expected compound forms, we stack the positions
			 * of the start and end of the data.
			 */
			if (unlikely(csp >= NR_CONS_STACK))
				goto cons_stack_overflow;
			cons_dp_stack[csp] = dp;
			cons_hdrlen_stack[csp] = hdr;
			if (!(flags & FLAG_INDEFINITE_LENGTH)) {
				cons_datalen_stack[csp] = datalen;
				datalen = dp + len;
			} else {
				cons_datalen_stack[csp] = 0;
			}
			csp++;
		}

		pr_debug("- TAG: %02x %zu%s\n",
			 tag, len, flags & FLAG_CONS ? " CONS" : "");
		tdp = dp;
	}

	/* Decide how to handle the operation */
	switch (op) {
	case ASN1_OP_MATCH_ANY_ACT:
	case ASN1_OP_MATCH_ANY_ACT_OR_SKIP:
	case ASN1_OP_COND_MATCH_ANY_ACT:
	case ASN1_OP_COND_MATCH_ANY_ACT_OR_SKIP:
		ret = actions[machine[pc + 1]](context, hdr, tag, data + dp, len);
		if (ret < 0)
			return ret;
		goto skip_data;

	case ASN1_OP_MATCH_ACT:
	case ASN1_OP_MATCH_ACT_OR_SKIP:
	case ASN1_OP_COND_MATCH_ACT_OR_SKIP:
		ret = actions[machine[pc + 2]](context, hdr, tag, data + dp, len);
		if (ret < 0)
			return ret;
		goto skip_data;

	case ASN1_OP_MATCH:
	case ASN1_OP_MATCH_OR_SKIP:
	case ASN1_OP_MATCH_ANY:
	case ASN1_OP_MATCH_ANY_OR_SKIP:
	case ASN1_OP_COND_MATCH_OR_SKIP:
	case ASN1_OP_COND_MATCH_ANY:
	case ASN1_OP_COND_MATCH_ANY_OR_SKIP:
	skip_data:
		if (!(flags & FLAG_CONS)) {
			if (flags & FLAG_INDEFINITE_LENGTH) {
				ret = asn1_find_indefinite_length(
					data, datalen, &dp, &len, &errmsg);
				if (ret < 0)
					goto error;
			} else {
				dp += len;
			}
			pr_debug("- LEAF: %zu\n", len);
		}
		pc += asn1_op_lengths[op];
		goto next_op;

	case ASN1_OP_MATCH_JUMP:
	case ASN1_OP_MATCH_JUMP_OR_SKIP:
	case ASN1_OP_COND_MATCH_JUMP_OR_SKIP:
		pr_debug("- MATCH_JUMP\n");
		if (unlikely(jsp == NR_JUMP_STACK))
			goto jump_stack_overflow;
		jump_stack[jsp++] = pc + asn1_op_lengths[op];
		pc = machine[pc + 2];
		goto next_op;

	case ASN1_OP_COND_FAIL:
		if (unlikely(!(flags & FLAG_MATCHED)))
			goto tag_mismatch;
		pc += asn1_op_lengths[op];
		goto next_op;

	case ASN1_OP_COMPLETE:
		if (unlikely(jsp != 0 || csp != 0)) {
			pr_err("ASN.1 decoder error: Stacks not empty at completion (%u, %u)\n",
			       jsp, csp);
			return -EBADMSG;
		}
		return 0;

	case ASN1_OP_END_SET:
	case ASN1_OP_END_SET_ACT:
		if (unlikely(!(flags & FLAG_MATCHED)))
			goto tag_mismatch;
	case ASN1_OP_END_SEQ:
	case ASN1_OP_END_SET_OF:
	case ASN1_OP_END_SEQ_OF:
	case ASN1_OP_END_SEQ_ACT:
	case ASN1_OP_END_SET_OF_ACT:
	case ASN1_OP_END_SEQ_OF_ACT:
		if (unlikely(csp <= 0))
			goto cons_stack_underflow;
		csp--;
		tdp = cons_dp_stack[csp];
		hdr = cons_hdrlen_stack[csp];
		len = datalen;
		datalen = cons_datalen_stack[csp];
		pr_debug("- end cons t=%zu dp=%zu l=%zu/%zu\n",
			 tdp, dp, len, datalen);
		if (datalen == 0) {
			/* Indefinite length - check for the EOC. */
			datalen = len;
			if (unlikely(datalen - dp < 2))
				goto data_overrun_error;
			if (data[dp++] != 0) {
				if (op & ASN1_OP_END__OF) {
					dp--;
					csp++;
					pc = machine[pc + 1];
					pr_debug("- continue\n");
					goto next_op;
				}
				goto missing_eoc;
			}
			if (data[dp++] != 0)
				goto invalid_eoc;
			len = dp - tdp - 2;
		} else {
			if (dp < len && (op & ASN1_OP_END__OF)) {
				datalen = len;
				csp++;
				pc = machine[pc + 1];
				pr_debug("- continue\n");
				goto next_op;
			}
			if (dp != len)
				goto cons_length_error;
			len -= tdp;
			pr_debug("- cons len l=%zu d=%zu\n", len, dp - tdp);
		}

		if (op & ASN1_OP_END__ACT) {
			unsigned char act;
			if (op & ASN1_OP_END__OF)
				act = machine[pc + 2];
			else
				act = machine[pc + 1];
			ret = actions[act](context, hdr, 0, data + tdp, len);
		}
		pc += asn1_op_lengths[op];
		goto next_op;

	case ASN1_OP_MAYBE_ACT:
		if (!(flags & FLAG_LAST_MATCHED)) {
			pc += asn1_op_lengths[op];
			goto next_op;
		}
	case ASN1_OP_ACT:
		ret = actions[machine[pc + 1]](context, hdr, tag, data + tdp, len);
		if (ret < 0)
			return ret;
		pc += asn1_op_lengths[op];
		goto next_op;

	case ASN1_OP_RETURN:
		if (unlikely(jsp <= 0))
			goto jump_stack_underflow;
		pc = jump_stack[--jsp];
		flags |= FLAG_MATCHED | FLAG_LAST_MATCHED;
		goto next_op;

	default:
		break;
	}

	/* Shouldn't reach here */
	pr_err("ASN.1 decoder error: Found reserved opcode (%u) pc=%zu\n",
	       op, pc);
	return -EBADMSG;

data_overrun_error:
	errmsg = "Data overrun error";
	goto error;
machine_overrun_error:
	errmsg = "Machine overrun error";
	goto error;
jump_stack_underflow:
	errmsg = "Jump stack underflow";
	goto error;
jump_stack_overflow:
	errmsg = "Jump stack overflow";
	goto error;
cons_stack_underflow:
	errmsg = "Cons stack underflow";
	goto error;
cons_stack_overflow:
	errmsg = "Cons stack overflow";
	goto error;
cons_length_error:
	errmsg = "Cons length error";
	goto error;
missing_eoc:
	errmsg = "Missing EOC in indefinite len cons";
	goto error;
invalid_eoc:
	errmsg = "Invalid length EOC";
	goto error;
length_too_long:
	errmsg = "Unsupported length";
	goto error;
indefinite_len_primitive:
	errmsg = "Indefinite len primitive not permitted";
	goto error;
tag_mismatch:
	errmsg = "Unexpected tag";
	goto error;
long_tag_not_supported:
	errmsg = "Long tag not supported";
error:
	pr_debug("\nASN1: %s [m=%zu d=%zu ot=%02x t=%02x l=%zu]\n",
		 errmsg, pc, dp, optag, tag, len);
	return -EBADMSG;
}
