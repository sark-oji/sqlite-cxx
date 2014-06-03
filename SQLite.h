#ifndef _SQLITE_H_
#define _SQLITE_H_

#include <string>
#include <functional>

#include <sqlite3.h>
#include "SQLiteStmt.h"

class SQLite {
public:
	/**
	 * コンストラクタ
	 */
	SQLite(const std::string &path);

	/**
	 * デストラクタ
	 */
	virtual ~SQLite();

	/**
	 * prepare
	 * @return SQLiteStmt(shared_ptr)
	 */
	SQLiteStmt prepare(const std::string &query);
	
	/**
	 * トランザクション
	 */
	void transaction(std::function<bool(void)> func);
private:
	sqlite3 *db = nullptr;

	/**
	 * トランザクション開始
	 */
	void begin();

	/**
	 * トランザクション終了
	 * commitします
	 */
	void commit();

	/**
	 * ロールバック
	 */
	void rollback();
};

#endif
