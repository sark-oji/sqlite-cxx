#ifndef _SQLITE_MANAGER_H_
#define _SQLITE_MANAGER_H_

#include <string>
#include <memory>
#include <map>

#include "SQLite.h"

class SQLiteManager {
public:
	/**
	 * シングルトン
	 * @return SQLiteManager instance
	 */
	static SQLiteManager *getInstance();
	
	/**
	 * DBのオープン
	 * @param path
	 * @return SQLite instance
	 */
	SQLite *open(const std::string &path);

	/**
	 * DBのクローズ
	 * @param path
	 * @return SQLite instance
	 */
	void close(const std::string &path);

	/**
	 * オープン済みDBを取得する
	 * @param path
	 * @return SQLite instance
	 */
	SQLite *get(const std::string &path);

private:
	SQLiteManager();
	virtual ~SQLiteManager();
	struct SQLiteManagerDeleter {
		void operator()(SQLiteManager *p) {
			delete p;
		}
	};
	typedef std::unique_ptr<SQLiteManager, SQLiteManagerDeleter> SQLiteManagerPointer;
	static SQLiteManagerPointer instance;
	
	std::map<std::string, std::unique_ptr<SQLite>> databases;
};

#endif
