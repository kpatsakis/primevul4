yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void *yyscanner, YR_COMPILER* compiler)
{
  YYUSE (yyvaluep);
  YYUSE (yyscanner);
  YYUSE (compiler);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yytype)
    {
          case 10: /* _IDENTIFIER_  */
#line 194 "grammar.y" /* yacc.c:1257  */
      { yr_free(((*yyvaluep).c_string)); }
#line 1339 "grammar.c" /* yacc.c:1257  */
        break;

    case 11: /* _STRING_IDENTIFIER_  */
#line 198 "grammar.y" /* yacc.c:1257  */
      { yr_free(((*yyvaluep).c_string)); }
#line 1345 "grammar.c" /* yacc.c:1257  */
        break;

    case 12: /* _STRING_COUNT_  */
#line 195 "grammar.y" /* yacc.c:1257  */
      { yr_free(((*yyvaluep).c_string)); }
#line 1351 "grammar.c" /* yacc.c:1257  */
        break;

    case 13: /* _STRING_OFFSET_  */
#line 196 "grammar.y" /* yacc.c:1257  */
      { yr_free(((*yyvaluep).c_string)); }
#line 1357 "grammar.c" /* yacc.c:1257  */
        break;

    case 14: /* _STRING_LENGTH_  */
#line 197 "grammar.y" /* yacc.c:1257  */
      { yr_free(((*yyvaluep).c_string)); }
#line 1363 "grammar.c" /* yacc.c:1257  */
        break;

    case 15: /* _STRING_IDENTIFIER_WITH_WILDCARD_  */
#line 199 "grammar.y" /* yacc.c:1257  */
      { yr_free(((*yyvaluep).c_string)); }
#line 1369 "grammar.c" /* yacc.c:1257  */
        break;

    case 19: /* _TEXT_STRING_  */
#line 200 "grammar.y" /* yacc.c:1257  */
      { yr_free(((*yyvaluep).sized_string)); }
#line 1375 "grammar.c" /* yacc.c:1257  */
        break;

    case 20: /* _HEX_STRING_  */
#line 201 "grammar.y" /* yacc.c:1257  */
      { yr_free(((*yyvaluep).sized_string)); }
#line 1381 "grammar.c" /* yacc.c:1257  */
        break;

    case 21: /* _REGEXP_  */
#line 202 "grammar.y" /* yacc.c:1257  */
      { yr_free(((*yyvaluep).sized_string)); }
#line 1387 "grammar.c" /* yacc.c:1257  */
        break;


      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}
