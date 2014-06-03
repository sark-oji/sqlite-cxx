#ifndef _SQLITE_STMT_H_
#define _SQLITE_STMT_H_

#include <string>
#include <vector>
#include <tuple>
#include <memory>
#include <map>

#include <sqlite3.h>

class _SQLiteStmt {
public:
	_SQLiteStmt(sqlite3_stmt *stmt);
	~_SQLiteStmt();

	/**
	 * 実行
	 */
	bool exec();

	/**
	 * 現在行のカラム数を返す
	 * @return column count
	 */
	int count();

	/**
	 * バインド
	 * @param index バインド変数の名前
	 * @param val 値
	 */
	void bind_name(const std::string &name, int val);
	/**
	 * バインド
	 * @param index バインド変数の名前
	 * @param val 値
	 */
	void bind_name(const std::string &name, unsigned int val);
	/**
	 * バインド
	 * @param name バインド変数の名前
	 * @param val 値
	 */
	void bind_name(const std::string &name, float val);
	/**
	 * バインド
	 * @param name バインド変数の名前
	 * @param val 値
	 */
	void bind_name(const std::string &name, const std::string &val);

	/**
	 * 可変長引数でbind
	 * 第一引数から順にindexをインクリメントしながらbindします
	 * @param int float char *をbind可
	 * ex) bind(1, "test", 1.5, 2);
	 */
	template<typename T>
	void bind(const T& t) {
		bind_index(index, t);
		index++;
	}
	template<typename First, typename... Rest>
	void bind(const First& first, const Rest&... rest) {
		bind(first);
		bind(rest...);
		// 最後までbindしたのでindexを初期化
		if ( sizeof...(rest) == 1 ) {
			index = 1;
		}
	}

	/**
	 * reset
	 * prepared statement再利用時にどうぞ
	 */
	void reset();

	/**
	 * 実行結果
	 * @param name カラム名
	 * @return 値へのポインタ
	 */
	void *column(const std::string &name);
	/**
	 * 実行結果 (int)
	 * @param name カラム名
	 * @return 値
	 */
	int column_int(const std::string &name);
	/**
	 * 実行結果 (float)
	 * @param name カラム名
	 * @return 値
	 */
	float column_float(const std::string &name);
	/**
	 * 実行結果 (char *)
	 * @param name カラム名
	 * @return 値
	 */
	std::string column_string(const std::string &name);

	/**
	 * カラムのサイズを得る
	 * @param name カラム名
	 * @return サイズ
	 */
	int column_bytes(const std::string &name);

private:
	sqlite3_stmt *stmt;
	int index;
	// tuple<name, type, value>
	std::vector<std::tuple<std::string, int, std::shared_ptr<void>>> record;

	/**
	 * バインド
	 * @param index バインド変数のインデックス 1〜
	 * @param val 値
	 */
	void bind_index(int index, int val);
	/**
	 * バインド
	 * @param index バインド変数のインデックス 1〜
	 * @param val 値
	 */
	void bind_index(int index, unsigned int val);
	/**
	 * バインド
	 * @param index バインド変数のインデックス 1〜
	 * @param val 値
	 */
	void bind_index(int index, float val);
	/**
	 * バインド
	 * @param index バインド変数のインデックス 1〜
	 * @param val 値
	 */
	void bind_index(int index, const std::string &val);

	/**
	 *
	 */
	int getKeyByName(const std::string &name);

	/**
	 *
	 */
	void *getValueByKey(int key);

	/**
	 *
	 */
	bool isExistColumn(const std::string &name);

	/**
	 *
	 */
	bool isExistKey(int key);
};

typedef std::unique_ptr<_SQLiteStmt> SQLiteStmt;

#endif
