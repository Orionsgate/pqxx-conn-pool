#include "pg_connection_pool.h"
#include <iostream>
PGConnectionPool::PGConnectionPool(PGConnectionInfoParser::ConnectionType
  connection_type, int max_connections)
  : connection_info_parser(connection_type), max_connections(max_connections)
{}

PGConnectionPool::PGConnectionPool(PGConnectionPool& rhs)
  : connection_info_parser(rhs.connection_info_parser),
  max_connections(rhs.max_connections)
{
  rhs.free_connections_mtx.lock();
  rhs.busy_connections_mtx.lock();
  free_connections = rhs.free_connections;
  rhs.free_connections_mtx.unlock();
  busy_connections = rhs.busy_connections;
  rhs.busy_connections_mtx.unlock();
}

std::shared_ptr<pqxx::connection> PGConnectionPool::getConnection() {
  while (true) {
    free_connections_mtx.lock();
    busy_connections_mtx.lock();
    if (!free_connections.empty()) {
      std::shared_ptr<pqxx::connection> connection = free_connections.at(0);
      busy_connections.push_back(connection);
      busy_connections_mtx.unlock();
      free_connections.erase(free_connections.begin());
      free_connections_mtx.unlock();
      return connection;
    } else if (free_connections.size() + busy_connections.size()
      < max_connections)
    {
      free_connections_mtx.unlock();
      bool conn_is_broken = true;
      std::shared_ptr<pqxx::connection> connection;
      while (conn_is_broken) {
        try {
          connection = std::make_shared<pqxx::connection>(
            "dbname = " + connection_info_parser.getDBName() +
            " user = " + connection_info_parser.getDBUser() +
            " password = " + connection_info_parser.getDBPass() +
            " host = " + connection_info_parser.getServerIP() +
            " port = " + connection_info_parser.getServerPort());
          busy_connections.push_back(connection);
          conn_is_broken = false;
        } catch (const pqxx::broken_connection& broken_connection) {
          std::cout << broken_connection.what();
          std::cout << "Broken connection.\n";
        }
      }
      busy_connections_mtx.unlock();
      return connection;
    } else {
      busy_connections_mtx.unlock();
      free_connections_mtx.unlock();
    }
    usleep(1000);
  }
}

void PGConnectionPool::destroyConnection(std::shared_ptr<pqxx::connection>
  connection)
{
  busy_connections_mtx.lock();
  for (auto conn_iter = busy_connections.begin();
    conn_iter != busy_connections.end(); conn_iter++)
  {
    if (connection == *conn_iter) {
      busy_connections.erase(conn_iter);
      break;
    }
  }
  busy_connections_mtx.unlock();
}

void PGConnectionPool::returnConnection(std::shared_ptr<pqxx::connection>
  connection)
{
  bool found = false;
  busy_connections_mtx.lock();
  for (auto conn_iter = busy_connections.begin();
    conn_iter != busy_connections.end(); conn_iter++)
  {
    if (connection == *conn_iter) {
      found = true;
      busy_connections.erase(conn_iter);
      break;
    }
  }
  busy_connections_mtx.unlock();
  if (found) {
    free_connections_mtx.lock();
    free_connections.push_back(connection);
    free_connections_mtx.unlock();
  } else {
    throw std::invalid_argument(std::string("Provided connection does not ")
      + "belong to the pool.");
  }
}
