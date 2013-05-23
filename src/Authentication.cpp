#include "Tokenizer.h"
#include "sqlite3.h"

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cctype>
#include<cstring>
#include<string>

void Tokenizer::logInChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "login") != NULL) {
		STR username(firstToken); 
		STR password(nextToken);
		STR command = "select name,password from User where name='" + username + "'"; 
		sqlite3_exec(db, command.c_str(), loadLogInAuthentication, &password, NULL);
		if (password == "changedsuccessfully") {
			fputs("login\tsuccessful\n", fout);
			STR command = "update User set status='login' where name='" + username + "'";
			sqlite3_exec(db, command.c_str(), NULL, NULL, NULL);
		} else fputs("login\tfailed\n", fout);
	}
}

void Tokenizer::logOutChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "logout") != NULL) {
		STR username(firstToken);
		if (isLogIn(db, username)) {
			fputs("logout\tsuccessful\n", fout);
			STR command = "update User set status='logout' where name='" + username + "'";
			sqlite3_exec(db, command.c_str(), NULL, NULL, NULL);
		} else fputs("logout\tfailed\n", fout);
	}
}

void Tokenizer::setPasswordChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "set-password") != NULL) {
		STR username(firstToken);
		nextToken;
		STR newpassword(nextToken);
		if (isLogIn(db, username)) {
			fputs("set-password\tsuccessful\n", fout);
			STR command = "update User set password='" + newpassword + "' where name='" + username + "'";
			sqlite3_exec(db, command.c_str(), NULL, NULL, NULL);
		} else fputs("set-password\tfailed\n", fout);
	}
}


