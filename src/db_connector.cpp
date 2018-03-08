#include "db_connector.h"
#include "logging.h"

DbConnector::DbConnector()
  : connection_(NULL),
    rows_(0) {
  connection_ = mysql_init(NULL);
}

DbConnector::~DbConnector() {
  if (connection_ != NULL) {
    mysql_close(connection_);
  }
}

bool DbConnector::Connect(const string& host, const string& user, const string& password, const string& dbname) { 
  if (mysql_real_connect(connection_, host.c_str(), user.c_str(), password.c_str(), dbname.c_str(), 0, NULL, 0) == NULL) {
    error("DbConnector connector '%s'@'%s' db[%s] failed, password: %s", user.c_str(), host.c_str(), dbname.c_str(), password.c_str()); 
    return false;
  }
  return true;
}

bool DbConnector::ExecInsertSql(const string& sql) {
  int ret = mysql_query(connection_, sql.c_str());
  if (ret != 0) {
    error("Exec insert sql %s failed", sql.c_str());
    return false;
  }

  rows_ = static_cast<unsigned long long>(mysql_affected_rows(connection_)); 

  return true;
}

std::vector<std::vector<std::string>> DbConnector::ExecSelectSql(const string& sql) {
  int ret = mysql_query(connection_, sql.c_str());
  if (ret != 0) {
    error("Exec insert sql %s failed", sql.c_str());
    return std::vector<std::vector<std::string>>();
  }

  MYSQL_RES* result = mysql_store_result(connection_);
  if (result == NULL) {
    fatal("DbConnector mysql_store_result return null");
  }

  rows_ = static_cast<unsigned long long>(mysql_num_rows(result)); 
  unsigned int columns = mysql_num_fields(result);

  MYSQL_ROW row;
  std::vector<std::vector<std::string>> query_res;

  if (rows_ > 0) {
     while ((row = mysql_fetch_row(result)) != NULL) {
      std::vector<std::string> str_row;
      for (unsigned int i=0; i<columns; ++i) {
        char value[256];
        sprintf(value, "%s", row[i]);
        std::string str(value);
        str_row.push_back(str);
      }
      query_res.push_back(str_row);
    }
  }

  mysql_free_result(result);

  return query_res;
}

