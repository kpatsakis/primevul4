  virtual void initializeServerHandshake() {
    fakeHandshake = new FakeServerHandshake(
        server->getNonConstConn(), getFizzServerContext());
  }