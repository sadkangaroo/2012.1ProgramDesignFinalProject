#include "Tokenizer.h"
#include "sqlite3.h"

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cctype>
#include<cstring>
#include<string>
#include<sstream>

#define printdots fputs("...\n", fout)

STR timeTrans(int sec) {
	time_t tt(sec);
	struct tm* ptm = localtime(&tt);
	char tmp[20];
	sprintf(tmp, "%04d/%02d/%02d", 1900 + ptm->tm_year, 1 + ptm->tm_mon, ptm->tm_mday);
	return STR(tmp);
}

int loadModifyInfo(void* db, int cnum, char** cvalue, char** cname) {
	if (strcmp(cvalue[1], "null") == 0) return 0;
	int t1 = atoi(cvalue[1]);
	int t2 = atoi(currentTime.c_str());
	if (t2 - t1 >= ReserveBookExpireSec) {
		sqlite3_exec((sqlite3*)db, STR("update Book set reservedby='null' where id='" + STR(cvalue[2]) + "'").c_str(), NULL, NULL, NULL);	
	}
	return 0;	
}

void updateReserveInfo(sqlite3* db) {
	sqlite3_exec(db, "select reservedby,reservedsince,id from Book", loadModifyInfo, db, NULL);
}

int loadUserNum(void* fout, int cnum, char** cvalue, char** cname) {
	fprintf((FILE*)fout, "%s user(s) found\n", cvalue[0]);
	return 0;
}

int loadUserListInfo(void* fout, int cnum, char** cvalue, char** cname) {
	fprintf((FILE*)fout, "%s\t%s\n", cvalue[0], cvalue[1]);
	return 0;
}

int loadBookNum(void* fout, int cnum, char** cvalue, char** cname) {
	fprintf((FILE*)fout, "%s book(s) found\n", cvalue[0]);
	return 0;
}

int loadBookListInfo(void* fout, int cnum, char** cvalue, char** cname) {
	for (int i = 0; i < cnum; ++i) {
		if (i != cnum - 1) fprintf((FILE*)fout, "%s\t", cvalue[i]);
		else fprintf((FILE*)fout, "%s\n", cvalue[i]);
	}
	return 0;	
}

int loadBorrowListInfo(void* fout, int cnum, char** cvalue, char** cname) {
	for (int i = 0; i < 4; ++i) 
		fprintf((FILE*)fout, "%s\t", cvalue[i]);
	int t0;
	if (strcmp(cvalue[5], "false") == 0) t0 = atoi(cvalue[4]);
	else t0 = atoi(cvalue[5]);
	int t1 = atoi(cvalue[4]);
	int t2 = t1 + BorrowedBookExpiredSec - OneDay;
	fprintf((FILE*)fout, "%s\t%s\n", timeTrans(t0).c_str(), timeTrans(t2).c_str());
	return 0;
}

int loadReserveListInfo(void* fout, int cnum, char** cvalue, char** cname) {
	for (int i = 0; i < 4; ++i)
		fprintf((FILE*)fout, "%s\t", cvalue[i]);
	int t0 = atoi(cvalue[4]);
	fprintf((FILE*)fout, "%s\n", timeTrans(t0).c_str());
	return 0;
}

int loadFull(void* fout, int cnum, char** cvalue, char** cname) {
	for (int i = 0; i < 4; ++i)
		fprintf((FILE*)fout, "%s\t", cvalue[i]);
	int t0 = atoi(cvalue[4]);
	if (strcmp(cvalue[4], "true") == 0) fprintf((FILE*)fout, "borrowable\t");
	else fprintf((FILE*)fout, "not borrowable\t");
	if (strcmp(cvalue[5], "null") != 0) fprintf((FILE*)fout, "borrowed\t");
	else fprintf((FILE*)fout, "not borrowed\t");
	if (strcmp(cvalue[6], "null") != 0) fprintf((FILE*)fout, "reserved\n");
	else fprintf((FILE*)fout, "not reserved\n");
	return 0;
}

void Tokenizer::listAllUsersChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "list-all-users") != NULL) {
		sqlite3_exec(db, "select count(name) from User", loadUserNum, fout, NULL);
		sqlite3_exec(db, "select name,generaltype from User order by generaltype,name", loadUserListInfo, fout, NULL);
		printdots;
	}
}

void Tokenizer::listAllBooksChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "list-all-books") != NULL) {
		updateReserveInfo(db);
		sqlite3_exec(db, "select count(id) from Book", loadBookNum, fout, NULL);
		sqlite3_exec(db, "select isbn,name,authors,id,borrowable,borrowedby,reservedby from Book order by isbn,id", loadBookListInfo, fout, NULL); 
		printdots;
	}
}

void Tokenizer::listBooksByIsbnChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "list-books-by-isbn") != NULL) {
		updateReserveInfo(db);
		firstToken;
		STR bookisbn(nextToken);
		sqlite3_exec(db, STR("select count(id) from Book where isbn='" + bookisbn + "'").c_str(), loadBookNum, fout, NULL);
		sqlite3_exec(db, STR("select isbn,name,authors,id,borrowable,borrowedby,reservedby from Book where isbn='" + bookisbn + "' order by isbn,id").c_str(), loadFull, fout, NULL);
		printdots;
	}
}

void Tokenizer::listBooksByNameChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "list-books-by-name") != NULL) {
		updateReserveInfo(db);
		firstToken;
		STR bookname(nextToken);
		sqlite3_exec(db, STR("select count(id) from Book where name like '%" + bookname + "%'").c_str(), loadBookNum, fout, NULL);
		sqlite3_exec(db, STR("select isbn,name,authors,id,borrowable,borrowedby,reservedby from Book where name like '%" + bookname + "\%' order by isbn,id").c_str(), loadFull, fout, NULL);
		printdots;
	}
}

void Tokenizer::listBooksByAuthorChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "list-books-by-author") != NULL) {
		updateReserveInfo(db);
		firstToken;
		STR bookauthor(nextToken);
		sqlite3_exec(db, STR("select count(id) from Book where authors like '%" + bookauthor + "%'").c_str(), loadBookNum, fout, NULL); 
		sqlite3_exec(db, STR("select isbn,name,authors,id,borrowable,borrowedby,reservedby from Book where authors like '%" + bookauthor + "\%' order by isbn,id").c_str(), loadFull, fout, NULL);
		printdots;
	}
}

void Tokenizer::listBooksByIndexChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "list-books-by-index") != NULL) {
		updateReserveInfo(db);
		firstToken;
		STR bookid(nextToken);
		sqlite3_exec(db, STR("select count(id) from Book where id='" + bookid + "'").c_str(), loadBookNum, fout, NULL);
		sqlite3_exec(db, STR("select isbn,name,authors,id,borrowable,borrowedby,reservedby from Book where id='" + bookid + "' order by isbn,id").c_str(), loadFull, fout, NULL);
		printdots;
	}
}

void Tokenizer::listUserBorrowedBooksChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "list-user-borrowed-books") != NULL) {
		firstToken;
		nextToken;
		STR username(nextToken);
		sqlite3_exec(db, STR("select count(id) from Book where borrowedby='" + username + "'").c_str(), loadBookNum, fout, NULL);
		sqlite3_exec(db, STR("select isbn,name,authors,id,borrowedsince,isrenewed from Book where borrowedby='" + username + "' order by isbn,id").c_str(), loadBorrowListInfo, fout, NULL); 
		printdots;
	}
}

void Tokenizer::listUserReversedBooksChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "list-user-reserved-books") != NULL) {
		updateReserveInfo(db);
		firstToken;
		nextToken;
		STR username(nextToken);
		sqlite3_exec(db, STR("select count(id) from Book where reservedby='" + username + "'").c_str(), loadBookNum, fout, NULL);
		sqlite3_exec(db, STR("select isbn,name,authors,id,reservedsince from Book where reservedby='" + username + "' order by isbn,id").c_str(), loadReserveListInfo, fout, NULL);
		printdots;
	}
}
