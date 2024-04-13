void cql_server::connection::on_connection_close()
{
    _server._notifier->unregister_connection(this);
}