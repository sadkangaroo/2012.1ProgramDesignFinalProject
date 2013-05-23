#include "Tokenizer.h"
#include "sqlite3.h"

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cctype>
#include<cstring>
#include<string>
#include<sstream>

void Tokenizer::borrowBookChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "borrow-book") != NULL) {
		STR username(firstToken);
		nextToken;
		STR bookisbn(nextToken);
		STR bookname(nextToken);
		STR bookid(nextToken);
		if (isLogInReader(db, username) && noPenalty(db, username) && notReachBorrowLimit(db, username) && isBorrowable(db, bookid) && avalible(db, username, bookid)) {
			fputs("borrow-book\tsuccessful\n", fout);
			STR command = "update Book set borrowedby='" + username + "' where id='" + bookid + "'";
			sqlite3_exec(db, command.c_str(), NULL, NULL, NULL);
			command = "update Book set isrenewed='false' where id='" + bookid + "'";
			sqlite3_exec(db, command.c_str(), NULL, NULL, NULL);
			command = "update Book set borrowedsince='" + currentTime + "' where id='" + bookid + "'";
			sqlite3_exec(db, command.c_str(), NULL, NULL, NULL);
			} else fputs("borrow-book\tfailed\n", fout);
	}
}

void Tokenizer::returnBookChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "return-book") != NULL) {
		STR username(firstToken);
		nextToken;
		STR bookisbn(nextToken);
		STR bookname(nextToken);
		STR bookid(nextToken);
		if (isLogInReader(db, username) && isBorrowedBy(db, bookid)) {
			fputs("return-book\tsuccessful\n", fout);
			STR command = "update Book set borrowedby='null' where id='" + bookid + "'";
			sqlite3_exec(db, command.c_str(), NULL, NULL, NULL);
		} else fputs("return-book\tfailed\n", fout);
	}
}

int loadBorrowedSinceInfo(void* ret, int cnum, char** cvalue, char** cname) {
	*((STR*)ret) = STR(cvalue[0]);
	return 0;
}

STR getBorrowedSince(sqlite3* db, STR bookid) {
	STR ret;
	sqlite3_exec(db, STR("select borrowedsince from Book where id='" + bookid + "'").c_str(), loadBorrowedSinceInfo, &ret, NULL);
	return ret;
}

void Tokenizer::renewBookChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "renew-book") != NULL) {
		STR username(firstToken);
		nextToken;
		STR bookisbn(nextToken);
		STR bookname(nextToken);
		STR bookid(nextToken);
		if (isLogInReader(db, username) && noPenalty(db, username) && isBorrowedByUser(db, bookid, username) && !isRenewed(db, bookid)) {
			fputs("renew-book\tsuccessful\n", fout);
			STR command = "update Book set isrenewed='" + getBorrowedSince(db, bookid) + "' where id='" + bookid + "'";
			sqlite3_exec(db, command.c_str(), NULL, NULL, NULL);
			command = "update Book set borrowedsince='" + currentTime + "' where id='" + bookid + "'";
			sqlite3_exec(db, command.c_str(), NULL, NULL, NULL);
		} else fputs("renew-book\tfailed\n", fout);
	}
}

void Tokenizer::reserveBookChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "reserve-book") != NULL) {
		STR username(firstToken);
		nextToken;
		STR bookisbn(nextToken);
		STR bookname(nextToken);
		STR bookid = searchForBook(db, bookisbn);
		if (bookid != "" && isLogInReader(db, username) && isTeacher(db, username) && noPenalty(db, username) && isBorrowable(db, bookid)) {
			fprintf(fout, "reserve-book\t%s\n", bookid.c_str());
			STR command = "update Book set reservedby='" + username + "' where id='" + bookid + "'";
			sqlite3_exec(db, command.c_str(), NULL, NULL, NULL);
			command = "update Book set reservedsince='" + currentTime + "' where id='" + bookid + "'";
			sqlite3_exec(db, command.c_str(), NULL, NULL, NULL);
		} else fputs("reserve-book\tfailed\n", fout);
	}
}
