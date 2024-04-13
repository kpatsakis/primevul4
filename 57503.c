static int backref_match_at_nested_level(regex_t* reg
	 , OnigStackType* top, OnigStackType* stk_base
	 , int ignore_case, int case_fold_flag
	 , int nest, int mem_num, UChar* memp, UChar** s, const UChar* send)
{
  UChar *ss, *p, *pstart, *pend = NULL_UCHARP;
  int level;
  OnigStackType* k;

  level = 0;
  k = top;
  k--;
  while (k >= stk_base) {
    if (k->type == STK_CALL_FRAME) {
      level--;
    }
    else if (k->type == STK_RETURN) {
      level++;
    }
    else if (level == nest) {
      if (k->type == STK_MEM_START) {
        if (mem_is_in_memp(k->u.mem.num, mem_num, memp)) {
          pstart = k->u.mem.pstr;
          if (pend != NULL_UCHARP) {
            if (pend - pstart > send - *s) return 0; /* or goto next_mem; */
            p  = pstart;
            ss = *s;

            if (ignore_case != 0) {
              if (string_cmp_ic(reg->enc, case_fold_flag,
                                pstart, &ss, (int )(pend - pstart)) == 0)
                return 0; /* or goto next_mem; */
            }
            else {
              while (p < pend) {
                if (*p++ != *ss++) return 0; /* or goto next_mem; */
              }
            }

            *s = ss;
            return 1;
          }
        }
      }
      else if (k->type == STK_MEM_END) {
        if (mem_is_in_memp(k->u.mem.num, mem_num, memp)) {
          pend = k->u.mem.pstr;
        }
      }
    }
    k--;
  }

  return 0;
}
