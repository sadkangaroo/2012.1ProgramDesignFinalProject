#include "LibraryDatabaseCreater.h"
#include "sqlite3.h"

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cctype>
#include<cstring>

LibraryDatabaseCreater::LibraryDatabaseCreater() {
	sqlite3_open("./Library_database.db", &db); 
}

LibraryDatabaseCreater::~LibraryDatabaseCreater() {
	sqlite3_close(db);
}

void LibraryDatabaseCreater::restart() {
	sqlite3_exec(db, "drop table User", NULL, NULL, NULL);
	sqlite3_exec(db, "drop table Book", NULL, NULL, NULL);
	createNewLibraryDatabase();
	addDefaultAdmin();
}

void LibraryDatabaseCreater::createNewLibraryDatabase() {
	createNewUserTable();
	createNewBookTable();
	sqlite3_exec(db, "create index indexForIsbn on Book(isbn)", NULL, NULL, NULL);
	sqlite3_exec(db, "create index indexForName on Book(name)", NULL, NULL, NULL);
	sqlite3_exec(db, "create index indexForBorrowedBy on Book(borrowedby)", NULL, NULL, NULL);
	sqlite3_exec(db, "create index indexForReservedBy on Book(reservedby)", NULL, NULL, NULL);
}

void LibraryDatabaseCreater::addDefaultAdmin() {
	sqlite3_exec(db, "insert into User values('admin', 'secret', 'ADMIN', 'ADMIN', 'logout')", NULL, NULL, NULL);
}

void LibraryDatabaseCreater::createNewUserTable() {
	sqlite3_exec(db, "create table User(name text primary key, password text, type text, generaltype text, status text)", NULL, NULL, NULL); 
	addDefaultAdmin();
}

void LibraryDatabaseCreater::createNewBookTable() {
	sqlite3_exec(db, "create table Book(isbn text, name text, authors text, id text primary key, borrowable text, borrowedby text, borrowedsince text, reservedby text, reservedsince text, isrenewed text)", NULL, NULL, NULL);
}
