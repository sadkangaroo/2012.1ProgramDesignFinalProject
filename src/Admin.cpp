#include "Tokenizer.h"
#include "sqlite3.h"

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cctype>
#include<cstring>
#include<string>

void Tokenizer::createUserChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "create-user") != NULL) {
		STR adminname(firstToken);
		nextToken;
		STR username(nextToken);
		STR password(nextToken);
		STR usertype(nextToken);
		STR  usergeneraltype;
		if (usertype == "ADMIN") usergeneraltype = "ADMIN"; else usergeneraltype = "READER";
		if (isLogInAdmin(db, adminname) && (usertype == "TEACHER" || usertype == "STUDENT" || usertype == "ADMIN") && !userExist(db, username)) {
			fputs("create-user\tsuccessful\n", fout);	
			STR command = "insert into User values('" + username + "', '" + password + "', '" + usertype + "', '" + usergeneraltype + "', 'logout')";
			sqlite3_exec(db, command.c_str(), NULL, NULL, NULL);
		} else fputs("create-user\tfailed\n", fout);
	}
}

void Tokenizer::removeUserChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "remove-user") != NULL) {
		STR adminname(firstToken); 
		nextToken;
		STR username(nextToken);
		STR usertype(nextToken);
		if (isLogInAdmin(db, adminname) && userExist(db, username)) {
			fputs("remove-user\tsuccessful\n", fout);
			STR command = "delete from User where name='" + username + "'";
			sqlite3_exec(db, command.c_str(), NULL, NULL, NULL);
		} else fputs("remove-user\tfailed\n", fout);
	}
}

void transAuthors(std::string &str) {
	char chineseComma[] = {-17, -68, -116, '\0'};
	char chineseComma2[] = {-93, -84, '\0'};
	char* p;
	char s[200]; int len = str.length();
	for (int i = 0; i < len; ++i) s[i] = str[i]; s[len] = '\0';
	while ((p = strstr(s, chineseComma)) != NULL) {
		p[0] = ' '; p[1] = ','; p[2] = ' ';
	}
	while ((p = strstr(s, chineseComma2)) != NULL) {
		p[0] = ' '; p[1] = ',';
	}
	p = strtok(s, ",\r\n");
	str = "";
	while (p != NULL) {
		std::string cnt(p);
		while (cnt != "" && cnt[0] == ' ') cnt.erase(0, 1);
		while (cnt != "" && cnt[cnt.length() - 1] == ' ') cnt.erase(cnt.length() - 1, 1);
		if (cnt != "") {
			if (str != "") str = str + ",";
			str = str + cnt;
		}
		p = strtok(NULL, ",\r\n");
	}
}

void Tokenizer::createBookChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "create-book") != NULL) {
		STR adminname(firstToken);
		nextToken;
		STR bookisbn(nextToken);
		STR bookname(nextToken);
		STR bookauthors(nextToken);
		STR bookid(nextToken);
		STR bookborrowable(nextToken);
		transAuthors(bookauthors);
		if (isLogInAdmin(db, adminname) && (bookborrowable == "true" || bookborrowable == "false") && !bookExist(db, bookid)) {
			fputs("create-book\tsuccessful\n", fout);
			STR command = "insert into Book values('" + bookisbn + "', '" + bookname + "', '" + bookauthors + "', '" + bookid + "', '" + bookborrowable + "', 'null', 'null', 'null', 'null', 'false')";
			sqlite3_exec(db, command.c_str(), NULL, NULL, NULL);
		} else fputs("create-book\tfailed\n", fout);
	}
}

void Tokenizer::removeBookChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "remove-book") != NULL) {
		STR adminname(firstToken);
		nextToken;
		STR bookisbn(nextToken);
		STR bookname(nextToken);
		STR bookid(nextToken);
		if (isLogInAdmin(db, adminname) && bookExist(db, bookid)) {
			fputs("remove-book\tsuccessful\n", fout);
			STR command = "delete from Book where id='" + bookid + "'";
			sqlite3_exec(db, command.c_str(), NULL, NULL, NULL);
		} else fputs("remove-book\tfailed\n", fout);

	}
}
