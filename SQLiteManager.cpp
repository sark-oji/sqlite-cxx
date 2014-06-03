#include "SQLiteManager.h"

using namespace std;

SQLiteManager::SQLiteManagerPointer SQLiteManager::instance;

/*
 * SQLiteManager
 */

SQLiteManager::SQLiteManager() {
}

SQLiteManager::~SQLiteManager() {
}

SQLiteManager *SQLiteManager::getInstance() {
	if ( !instance ) {
		instance = SQLiteManagerPointer(new SQLiteManager());
	}
	return instance.get();
}

SQLite *SQLiteManager::open(const string &path) {
	if ( databases.count(path) == 0 ) {
		unique_ptr<SQLite> sqlite(new SQLite(path));
		databases[path] = std::move(sqlite);
	}
	return databases[path].get();
}

void SQLiteManager::close(const string &path) {
	auto it = databases.find(path);
	if ( it != databases.end() ) {
		databases.erase(it);
	}
}

SQLite *SQLiteManager::get(const string &path) {
	SQLite *sqlite = nullptr;
	if ( databases.count(path) == 0 ) {
		sqlite = databases[path].get();
	}
	return sqlite;
}
