#ifndef FRONTENDAPPLICATION_HPP
#define FRONTENDAPPLICATION_HPP

#include "gui/containers/BamblooKeyboard.hpp"
#include "sqlite3.h"
#include "touchgfx/Callback.hpp"
#include <gui_generated/common/FrontendApplicationBase.hpp>

class FrontendHeap;

using namespace touchgfx;

class FrontendApplication : public FrontendApplicationBase {
public:
  FrontendApplication(Model &m, FrontendHeap &heap);
  virtual ~FrontendApplication() {}

  virtual void handleTickEvent() {
    model.tick();
    FrontendApplicationBase::handleTickEvent();
  }

  void showKeyboard(bool passwordMode = false);
  void hideKeyboard();
  bool isKeyboardVisible() const;
  void setKeyboardBuffer(Unicode::UnicodeChar *buffer, uint16_t size);
  void setKeyboardCallback(GenericCallback<Unicode::UnicodeChar> &callback);
  void attachKeyboardToCurrentScreen();

  static FrontendApplication *getInstance() {
    return static_cast<FrontendApplication *>(Application::getInstance());
  }

  int initDatabase() {
    return sqlite3_initialize() || sqlite3_open("bambloo.db", &sqlite);
  }

  void testDatabase() {
    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_open("test.db", &sqlite);

    const char *sql_create_table = "CREATE TABLE IF NOT EXISTS users ("
                                   "id INTEGER PRIMARY KEY,"
                                   "name TEXT NOT NULL,"
                                   "data BLOB);";

    rc = sqlite3_exec(sqlite, sql_create_table, 0, 0, 0);
    if (rc != SQLITE_OK) {
      // printf("SQL error (Create Table): %s\n", sqlite3_errmsg(sqlite));
      sqlite3_close(sqlite);
      return;
    }

    const char *sql_insert = "INSERT INTO users (name, data) VALUES (?, ?);";
    rc = sqlite3_prepare_v2(sqlite, sql_insert, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
      // printf("Failed to prepare statement: %s\n", sqlite3_errmsg(sqlite));
      sqlite3_close(sqlite);
      return;
    }

    sqlite3_bind_text(stmt, 1, "test", -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 2, "Hello Man", 8, SQLITE_STATIC);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    const char *sql_select =
        "SELECT name, data FROM users WHERE name = 'test' LIMIT 1;";
    rc = sqlite3_prepare_v2(sqlite, sql_select, -1, &stmt, NULL);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
      // 读取文本名字
      const unsigned char *ret_name = sqlite3_column_text(stmt, 0);

      // 🌟 读取 BLOB 数据及其字节长度
      const uint8_t *ret_blob = (const uint8_t *)sqlite3_column_blob(stmt, 1);
      int ret_blob_len = sqlite3_column_bytes(stmt, 1);
    } else {
    }
  }

private:
  sqlite3 *sqlite;
  BamblooKeyboard globalKeyboard;
};

#endif // FRONTENDAPPLICATION_HPP
