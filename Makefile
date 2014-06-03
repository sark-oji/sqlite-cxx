all: sample

sample: SQLite.o SQLiteStmt.o SQLiteManager.o sample.o
	clang++ -o sample -Os -std=c++11 -stdlib=libc++ -lc++abi -lsqlite3 SQLite.o SQLiteStmt.o SQLiteManager.o sample.o

.SUFFIXES: .c .cpp .o
.cpp.o: SQLite.cpp SQLiteStmt.cpp SQLiteManager.cpp sample.cpp
	clang++ -c -std=c++11 -stdlib=libc++ $<

.PHONY: clean
clean:
	rm -f sample *.o sample.db
