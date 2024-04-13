iperf_free_stream(struct iperf_stream *sp)
{
    struct iperf_interval_results *irp, *nirp;

    /* XXX: need to free interval list too! */
    munmap(sp->buffer, sp->test->settings->blksize);
    close(sp->buffer_fd);
    if (sp->diskfile_fd >= 0)
	close(sp->diskfile_fd);
    for (irp = TAILQ_FIRST(&sp->result->interval_results); irp != TAILQ_END(sp->result->interval_results); irp = nirp) {
        nirp = TAILQ_NEXT(irp, irlistentries);
        free(irp);
    }
    free(sp->result);
    if (sp->send_timer != NULL)
	tmr_cancel(sp->send_timer);
    free(sp);
}
