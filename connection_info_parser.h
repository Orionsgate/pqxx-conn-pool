#ifndef CONNECTION_INFO_PARSER_H
#define CONNECTION_INFO_PARSER_H

#include <fstream>
#include <string>

class ConnectionInfoParser {
  private:
    std::string connection_info[5];
  public:
    enum ConnectionType {READ, WRITE};
    ConnectionInfoParser(ConnectionType connection_type);
    std::string getServerIP() const;
    std::string getServerPort() const;
    std::string getDBName() const;
    std::string getDBUser() const;
    std::string getDBPass() const;
};

#endif
