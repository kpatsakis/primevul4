moffset(struct magic_set *ms, struct magic *m)
{
  	switch (m->type) {
  	case FILE_BYTE:
		return CAST(int32_t, (ms->offset + sizeof(char)));

  	case FILE_SHORT:
  	case FILE_BESHORT:
  	case FILE_LESHORT:
		return CAST(int32_t, (ms->offset + sizeof(short)));

  	case FILE_LONG:
  	case FILE_BELONG:
  	case FILE_LELONG:
  	case FILE_MELONG:
		return CAST(int32_t, (ms->offset + sizeof(int32_t)));

  	case FILE_QUAD:
  	case FILE_BEQUAD:
  	case FILE_LEQUAD:
		return CAST(int32_t, (ms->offset + sizeof(int64_t)));

  	case FILE_STRING:
  	case FILE_PSTRING:
  	case FILE_BESTRING16:
  	case FILE_LESTRING16:
		if (m->reln == '=' || m->reln == '!')
			return ms->offset + m->vallen;
		else {
			union VALUETYPE *p = &ms->ms_value;
			uint32_t t;

			if (*m->value.s == '\0')
				p->s[strcspn(p->s, "\n")] = '\0';
			t = CAST(uint32_t, (ms->offset + strlen(p->s)));
			if (m->type == FILE_PSTRING)
				t += (uint32_t)file_pstring_length_size(m);
			return t;
		}

	case FILE_DATE:
	case FILE_BEDATE:
	case FILE_LEDATE:
	case FILE_MEDATE:
		return CAST(int32_t, (ms->offset + sizeof(uint32_t)));

	case FILE_LDATE:
	case FILE_BELDATE:
	case FILE_LELDATE:
	case FILE_MELDATE:
		return CAST(int32_t, (ms->offset + sizeof(uint32_t)));

	case FILE_QDATE:
	case FILE_BEQDATE:
	case FILE_LEQDATE:
		return CAST(int32_t, (ms->offset + sizeof(uint64_t)));

	case FILE_QLDATE:
	case FILE_BEQLDATE:
	case FILE_LEQLDATE:
		return CAST(int32_t, (ms->offset + sizeof(uint64_t)));

  	case FILE_FLOAT:
  	case FILE_BEFLOAT:
  	case FILE_LEFLOAT:
		return CAST(int32_t, (ms->offset + sizeof(float)));

  	case FILE_DOUBLE:
  	case FILE_BEDOUBLE:
  	case FILE_LEDOUBLE:
		return CAST(int32_t, (ms->offset + sizeof(double)));

	case FILE_REGEX:
		if ((m->str_flags & REGEX_OFFSET_START) != 0)
			return CAST(int32_t, ms->search.offset);
		else
			return CAST(int32_t, (ms->search.offset +
			    ms->search.rm_len));

	case FILE_SEARCH:
		if ((m->str_flags & REGEX_OFFSET_START) != 0)
			return CAST(int32_t, ms->search.offset);
		else
			return CAST(int32_t, (ms->search.offset + m->vallen));

	case FILE_CLEAR:
	case FILE_DEFAULT:
	case FILE_INDIRECT:
		return ms->offset;

	default:
		return 0;
	}
}
