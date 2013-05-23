#ifndef _LibraryDatabaseCreater_H_
#define _LibraryDatabaseCreater_H_

#include "sqlite3.h"

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cctype>
#include<cstring>

class LibraryDatabaseCreater {
	public:
		LibraryDatabaseCreater();
		~LibraryDatabaseCreater();
		void restart();
		void createNewLibraryDatabase();
		void createNewUserTable();
		void createNewBookTable();	
		void addDefaultAdmin();
	private:
		sqlite3* db;
};

#endif /* Not _LibraryDatabaseCreater_H_ */
