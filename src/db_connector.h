#ifndef DB_CONNECTOR_H
#define DB_CONNECTOR_H

#include <mysql/mysql.h>
#include <vector>
#include <string>

using std::string;

class DbConnector {
 public:
  DbConnector();
  ~DbConnector();

  DbConnector(const DbConnector&) = delete;
  DbConnector& operator=(const DbConnector&) = delete;

  bool Connect(const string& host, const string& user, const string& password, const string& dbname);

  bool ExecInsertSql(const string& sql);
  std::vector<std::vector<std::string>> ExecSelectSql(const string& sql);

  unsigned long long GetNumOfRows() const {
    return rows_;
  }

 private:
  MYSQL* connection_;
  unsigned long long rows_;
};

#endif  // DB_CONNECTOR_H

