static void on_syntax_error(struct pj_scanner *scanner)
{
    PJ_UNUSED_ARG(scanner);
    PJ_THROW(EX_SYNTAX_ERROR);
}