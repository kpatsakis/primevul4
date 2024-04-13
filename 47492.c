struct proclistlist *pll_create(void){
  struct proclistlist *pll=calloc(1,sizeof(struct proclistlist));
  pll->proclist=get_proclist(&pll->length);
  return pll;
}
