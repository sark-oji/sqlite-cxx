#include "SQLiteStmt.h"

#include <iostream>
#include <sstream>
using namespace std;

/*
 * SQLiteStmt
 */
_SQLiteStmt::_SQLiteStmt(sqlite3_stmt *stmt) {
	this->stmt = stmt;
	this->index = 1;
}

_SQLiteStmt::~_SQLiteStmt() {
	sqlite3_finalize(stmt);
}

bool _SQLiteStmt::exec() {
	// 初期化
	record.clear();
	
	int result = sqlite3_step(stmt);
	if ( result == SQLITE_ROW ) {
		int column_count = sqlite3_column_count(stmt);
		for ( int i = 0; i < column_count; i++ ) {
			// カラム名を覚えておく
			string name = sqlite3_column_name(stmt, i);
				
			// 値の取得、保持
			int type = sqlite3_column_type(stmt, i);
			shared_ptr<void> value;
			if ( type == SQLITE_INTEGER ) {
				int temp = sqlite3_column_int(stmt, i);
				value = make_shared<int>(temp);
			}
			else if ( type == SQLITE_FLOAT ) {
				float temp = sqlite3_column_double(stmt, i);
				value = make_shared<float>(temp);
			}
			else if ( type == SQLITE_BLOB ) {
				int size = sqlite3_column_bytes(stmt, i);
				value = shared_ptr<unsigned char>(new unsigned char[size], default_delete<unsigned char[]>());
				memcpy(value.get(), sqlite3_column_text(stmt, i), size);
			}
			else if ( type == SQLITE_TEXT ) {
				int size = sqlite3_column_bytes(stmt, i) + 1;
				value = shared_ptr<char>(new char[size], default_delete<char[]>());
				memcpy(value.get(), sqlite3_column_text(stmt, i), size);
			}
			else {
				value = nullptr;
			}
			record.push_back(make_tuple(name, type, value));
		}
	}
	else if ( result == SQLITE_DONE ) {
		return false;
	}
	else {
		throw sqlite3_errmsg(sqlite3_db_handle(stmt));
	}
	
	return true;
}

int _SQLiteStmt::count() {
	return sqlite3_data_count(stmt);
}

void _SQLiteStmt::bind_name(const string &name, int val) {
	ostringstream os;
	os << "@" << name;
	string temp = os.str();
	int index = sqlite3_bind_parameter_index(stmt, temp.c_str());
	if ( index != 0 ) {
		bind_index(index , val);
	}
	else {
		ostringstream os;
		os << "no matching parameter is found: " << name;
		throw os.str().c_str();
	}
}
	
void _SQLiteStmt::bind_name(const string &name, unsigned int val) {
	ostringstream os;
	os << "@" << name;
	string temp = os.str();
	int index = sqlite3_bind_parameter_index(stmt, temp.c_str());
	if ( index != 0 ) {
		bind_index(index , val);
	}
	else {
		ostringstream os;
		os << "no matching parameter is found: " << name;
		throw os.str().c_str();
	}
}

void _SQLiteStmt::bind_name(const string &name, float val) {
	ostringstream os;
	os << "@" << name;
	string temp = os.str();
	int index = sqlite3_bind_parameter_index(stmt, temp.c_str());
	if ( index != 0 ) {
		bind_index(index , val);
	}
	else {
		ostringstream os;
		os << "no matching parameter is found: " << name;
		throw os.str().c_str();
	}
}

void _SQLiteStmt::bind_name(const string &name, const string &val) {
	ostringstream os;
	os << "@" << name;
	string temp = os.str();
	int index = sqlite3_bind_parameter_index(stmt, temp.c_str());
	bind_index(index, val);
}

void _SQLiteStmt::reset() {
	sqlite3_reset(stmt);
	index = 1;
}

void *_SQLiteStmt::column(const string &name) {
	int key = getKeyByName(name);
	if ( key == -1 ) {
		return 0;
	}
	return getValueByKey(key);
}

int _SQLiteStmt::column_int(const string &name) {
	void *temp = column(name);
	int key = getKeyByName(name);
	if ( key != -1 ) {
		int type = get<1>(record[key]);
		int value;
		switch ( type ) {
		case SQLITE_INTEGER:
			value = *((int *)temp);
			break;
		case SQLITE_FLOAT:
			value = (int)*((float *)temp);
			break;
		}
		return value;
	}
	return 0;
}

float _SQLiteStmt::column_float(const string &name) {
	void *temp = column(name);
	int key = getKeyByName(name);
	if ( key != -1 ) {
		int type = get<1>(record[key]);
		float value;
		switch ( type ) {
		case SQLITE_INTEGER:
			value = (float)*((int *)temp);
			break;
		case SQLITE_FLOAT:
			value = *((float *)temp);
			break;
		}
		return value;
	}
	return 0;
}

string _SQLiteStmt::column_string(const string &name) {
	const char *temp = (const char *)column(name);
	string value = temp ? temp : "";
		
	return value;
}

int _SQLiteStmt::column_bytes(const string &name) {
	int size = 0;
	int key = getKeyByName(name);
	if ( key != -1 ) {
		size = sqlite3_column_bytes(stmt, key);
	}
	return size;
}

void _SQLiteStmt::bind_index(int index, int val) {
	sqlite3_bind_int(stmt, index, val);
}

void _SQLiteStmt::bind_index(int index, unsigned int val) {
	sqlite3_bind_int(stmt, index, val);
}

void _SQLiteStmt::bind_index(int index, float val) {
	sqlite3_bind_double(stmt, index , val);
}

void _SQLiteStmt::bind_index(int index, const string &val) {
	sqlite3_bind_text(stmt, index, val.c_str(), -1, SQLITE_STATIC);
}

int _SQLiteStmt::getKeyByName(const string &name) {
	int key = -1;
	for ( auto it = record.begin(); it != record.end(); it++ ) {
		if ( get<0>(*it) == name ) {
			key = it - record.begin();
			break;
		}
	}
	if ( key == -1 ) {
		ostringstream os;
		os << name << " is not exist.";
		throw os.str().c_str();
	}
	return key;
}

void *_SQLiteStmt::getValueByKey(int key) {
	void *value = nullptr;
	if ( isExistKey(key) ) {
		value = get<2>(record[key]).get();
	}
	else {
		ostringstream os;
		os << "index " << key << " is out of range.";
		throw os.str().c_str();
	}
	return value;
}

bool _SQLiteStmt::isExistColumn(const string &name) {
	bool exist = false;
	for ( auto it = record.begin(); it != record.end(); it++ ) {
		if ( get<0>(*it) == name ) {
			exist = true;
			break;
		}
	}
	return exist;
}
	
bool _SQLiteStmt::isExistKey(int key) {
	bool exist = true;
	if ( record.size() < key ) {
		return false;
	}
	return exist;
}
