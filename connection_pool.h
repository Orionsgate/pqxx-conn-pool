#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <atomic>
#include <memory>
#include <mutex>
#include <pqxx/pqxx>
#include <unistd.h>
#include <vector>
#include <stdexcept>

#include "connection_info_parser.h"

class ConnectionPool {
  private:
    ConnectionInfoParser connection_info_parser;
    const int max_connections;
    std::mutex free_connections_mtx;
    std::mutex busy_connections_mtx;
    std::vector<std::shared_ptr<pqxx::connection>> free_connections;
    std::vector<std::shared_ptr<pqxx::connection>> busy_connections;
  public:
    ConnectionPool(ConnectionInfoParser::ConnectionType connection_type,
      int max_connections);
    ConnectionPool(ConnectionPool& rhs);
    std::shared_ptr<pqxx::connection> getConnection();
    void returnConnection(std::shared_ptr<pqxx::connection> connection);
};

#endif
