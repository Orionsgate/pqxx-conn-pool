#include "pg_connection_info_parser.h"

#include <filesystem>

PGConnectionInfoParser::PGConnectionInfoParser(ConnectionType connection_type) {
  std::ifstream connection_info_file;
  if (connection_type == ConnectionType::READ) {
    connection_info_file.open("conf/db_connection.ini");
  } else {
    connection_info_file.open("conf/db_connection_write.ini");
  }
  int line_index = 0;
  for (std::string line; getline(connection_info_file, line);) {
    switch (line_index) {
      case 1:
        connection_info[0] = line;
        break;
      case 2:
        connection_info[1] = line;
        break;
      case 3:
        connection_info[2] = line;
        break;
      case 4:
        connection_info[3] = line;
        break;
      case 5:
        connection_info[4] = line;
        break;
    }
    line_index++;
  }
}

std::string PGConnectionInfoParser::getServerIP() const {
  return connection_info[0];
}

std::string PGConnectionInfoParser::getServerPort() const {
  return connection_info[1];
}

std::string PGConnectionInfoParser::getDBName() const {
  return connection_info[2];
}

std::string PGConnectionInfoParser::getDBUser() const {
  return connection_info[3];
}

std::string PGConnectionInfoParser::getDBPass() const {
  return connection_info[4];
}
