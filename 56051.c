isis_print_metric_block(netdissect_options *ndo,
                        const struct isis_metric_block *isis_metric_block)
{
    ND_PRINT((ndo, ", Default Metric: %d, %s",
           ISIS_LSP_TLV_METRIC_VALUE(isis_metric_block->metric_default),
           ISIS_LSP_TLV_METRIC_IE(isis_metric_block->metric_default) ? "External" : "Internal"));
    if (!ISIS_LSP_TLV_METRIC_SUPPORTED(isis_metric_block->metric_delay))
        ND_PRINT((ndo, "\n\t\t  Delay Metric: %d, %s",
               ISIS_LSP_TLV_METRIC_VALUE(isis_metric_block->metric_delay),
               ISIS_LSP_TLV_METRIC_IE(isis_metric_block->metric_delay) ? "External" : "Internal"));
    if (!ISIS_LSP_TLV_METRIC_SUPPORTED(isis_metric_block->metric_expense))
        ND_PRINT((ndo, "\n\t\t  Expense Metric: %d, %s",
               ISIS_LSP_TLV_METRIC_VALUE(isis_metric_block->metric_expense),
               ISIS_LSP_TLV_METRIC_IE(isis_metric_block->metric_expense) ? "External" : "Internal"));
    if (!ISIS_LSP_TLV_METRIC_SUPPORTED(isis_metric_block->metric_error))
        ND_PRINT((ndo, "\n\t\t  Error Metric: %d, %s",
               ISIS_LSP_TLV_METRIC_VALUE(isis_metric_block->metric_error),
               ISIS_LSP_TLV_METRIC_IE(isis_metric_block->metric_error) ? "External" : "Internal"));

    return(1); /* everything is ok */
}
