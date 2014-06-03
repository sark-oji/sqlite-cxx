#include "SQLite.h"

#include <iostream>
using namespace std;

SQLite::SQLite(const string &path) {
	int result = sqlite3_open_v2(path.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
	if ( result != SQLITE_OK ) {
		throw sqlite3_errmsg(db);
	}
}

SQLite::~SQLite() {
	sqlite3_close(db);
}

SQLiteStmt SQLite::prepare(const std::string &query) {
	sqlite3_stmt *stmt;
	int result = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
	if ( result != SQLITE_OK ) {
		throw sqlite3_errmsg(db);
	}
	sqlite3_reset(stmt);
	
	return unique_ptr<_SQLiteStmt>(new _SQLiteStmt(stmt));
}

void SQLite::transaction(std::function<bool(void)> func) {
	try {
		begin();
		bool result = func();
		if ( result ) {
			commit();
		}
		else {
			rollback();
		}
	}
	catch ( const char *e ) {
		rollback();
		throw;
	}
}

void SQLite::begin() {
	sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);
}

void SQLite::commit() {
	sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
}

void SQLite::rollback() {
	sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
}
