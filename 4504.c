  FakeServerHandshake* getFakeHandshakeLayer() {
    return CHECK_NOTNULL(dynamic_cast<FakeServerHandshake*>(
        server->getNonConstConn().handshakeLayer.get()));
  }