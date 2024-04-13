static int string_cmp_ic(OnigEncoding enc, int case_fold_flag,
			 UChar* s1, UChar** ps2, int mblen)
{
  UChar buf1[ONIGENC_MBC_CASE_FOLD_MAXLEN];
  UChar buf2[ONIGENC_MBC_CASE_FOLD_MAXLEN];
  UChar *p1, *p2, *end1, *s2, *end2;
  int len1, len2;

  s2   = *ps2;
  end1 = s1 + mblen;
  end2 = s2 + mblen;
  while (s1 < end1) {
    len1 = ONIGENC_MBC_CASE_FOLD(enc, case_fold_flag, &s1, end1, buf1);
    len2 = ONIGENC_MBC_CASE_FOLD(enc, case_fold_flag, &s2, end2, buf2);
    if (len1 != len2) return 0;
    p1 = buf1;
    p2 = buf2;
    while (len1-- > 0) {
      if (*p1 != *p2) return 0;
      p1++;
      p2++;
    }
  }

  *ps2 = s2;
  return 1;
}
