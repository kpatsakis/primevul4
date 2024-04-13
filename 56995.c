httpd_destroy_conn( httpd_conn* hc )
{
    if (hc->initialized) {
	free(hc->read_buf);
	free(hc->decodedurl);
	free(hc->origfilename);
	free(hc->expnfilename);
	free(hc->encodings);
	free(hc->pathinfo);
	free(hc->query);
	free(hc->accept);
	free(hc->accepte);
	free(hc->reqhost);
	free(hc->hostdir);
	free(hc->remoteuser);
	free(hc->response);
#ifdef TILDE_MAP_2
	free(hc->altdir);
#endif /* TILDE_MAP_2 */
	hc->initialized = 0;
    }
}
