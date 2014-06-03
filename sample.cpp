#include <iostream>
using namespace std;

#include "SQLiteManager.h"

int main(int argc, char *argv[]) {
	/* インスタンスを得る */
	SQLiteManager *sqlite_manager = SQLiteManager::getInstance();
	SQLite *sqlite = sqlite_manager->open("sample.db");

	try {
		/* トランザクションを切って処理開始 */
		sqlite->transaction([&]() {
			SQLiteStmt stmt;

			/* 使ってみる */
			// テーブル作成
			// ステートメントの準備
			stmt = sqlite->prepare("CREATE TABLE test ( id INTEGER PRIMARY KEY AUTOINCREMENT, value1 FLOAT, value2 TEXT )");
			// 実行
			stmt->exec();

			// insertその1
			stmt = sqlite->prepare("INSERT INTO test ( value1, value2 ) VALUES ( @value1, @value2 )");
			// 名前でbind
			stmt->bind_name("value1", 1);
			stmt->bind_name("value2", "text");
			stmt->exec();
	
			// insertその2
			stmt = sqlite->prepare("INSERT INTO test ( value1, value2 ) VALUES ( ?, ? )");
			// 可変長引数でbind
			stmt->bind(2, "text");
			stmt->exec();

			// insertその3
			stmt->reset();
			// stmt再利用
			stmt->bind(3, "text");
			stmt->exec();
	
			// select
			stmt = sqlite->prepare("SELECT * FROM test");
			while ( stmt->exec() ) {
				cout << stmt->column_int("id") << endl;
				cout << stmt->column_int("value1") << endl;
				cout << stmt->column_string("value2") << endl;
				cout << endl;
			}

			// drop
			stmt = sqlite->prepare("DROP TABLE IF EXISTS test");
			stmt->exec();
			return true;
		});
	}
	catch ( const char *e ) {
		cerr << e << endl;
	}
	return 0;
}
