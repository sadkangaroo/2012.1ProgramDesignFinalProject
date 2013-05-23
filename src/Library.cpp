#include "sqlite3.h"
#include "LibraryDatabaseCreater.h"
#include "Tokenizer.h"

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cctype>
#include<cstring>

int main() {

	const char inputFile[] = "./in.txt";
	const char outputFile[] = "./out.txt";

	FILE* fin = fopen(inputFile, "rb");
	FILE* fout = fopen(outputFile, "wb");

	LibraryDatabaseCreater *creater = NULL;
	Tokenizer *tokenizer = new Tokenizer;

	if (access("./Library_database.db", 0) == -1) {
		creater = new LibraryDatabaseCreater;
		creater->createNewLibraryDatabase();
	}else creater = new LibraryDatabaseCreater;

	sqlite3* db = NULL;

	sqlite3_open("./Library_database.db", &db); 

	char str[100 + 5];
	while (tokenizer->load(fin)) {
		if (tokenizer->getTokens(db, fout))
			creater->restart();
	}

	sqlite3_close(db);
	
	fclose(fin);
	fclose(fout);

	return 0;

}
