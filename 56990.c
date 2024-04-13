free_httpd_server( httpd_server* hs )
{
    free(hs->binding_hostname);
    free(hs->cwd);
    free(hs->cgi_pattern);
    free(hs->charset);
    free(hs->p3p);
    free(hs->url_pattern);
    free(hs->local_pattern);
    free(hs);
}
