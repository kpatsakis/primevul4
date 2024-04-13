  void deliverData(
      Buf data,
      bool writes = true,
      folly::SocketAddress* peer = nullptr) {
    deliverDataWithoutErrorCheck(std::move(data), writes, peer);
    if (server->getConn().localConnectionError) {
      bool idleTimeout = false;
      const LocalErrorCode* localError =
          server->getConn().localConnectionError->first.asLocalErrorCode();
      if (localError) {
        idleTimeout = (*localError == LocalErrorCode::IDLE_TIMEOUT);
      }
      if (!idleTimeout) {
        throw std::runtime_error(
            toString(server->getConn().localConnectionError->first));
      }
    }
  }