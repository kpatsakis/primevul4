void onServerReadData(
    QuicServerConnectionState& conn,
    ServerEvents::ReadData& readData) {
  switch (conn.state) {
    case ServerState::Open:
      onServerReadDataFromOpen(conn, readData);
      return;
    case ServerState::Closed:
      onServerReadDataFromClosed(conn, readData);
      return;
  }
}