#ifndef _Tokenizer_H_
#define _Tokenizer_H_

#include "sqlite3.h"

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cctype>
#include<cstring>
#include<string>
#include<ctime>

#define STR std::string 
#define deli "\t\n\r"
#define firstToken strtok(buffer, deli)
#define nextToken strtok(NULL, deli)

extern STR currentTime;

const int Max_Buffer_Size = 500 + 5;
const int BorrowedBookExpiredSec = 2592000;
const int StudentBorrowLimit = 5;
const int TeacherBorrowLimit = 10;
const int ReserveBookExpireSec = 86400;
const int OneDay = 86400;

int loadLogInAuthentication(void*, int, char**, char**);

class Tokenizer {
	public:
		bool load(FILE* in);
		int getTokens(sqlite3*, FILE*);
	private:
		int isLogInAdmin(sqlite3* db, STR adminname);
		int isLogInReader(sqlite3* db, STR username);
		int isLogIn(sqlite3* db, STR username);
		int isTeacher(sqlite3* db, STR username);
		int userExist(sqlite3* db, STR username);
		int bookExist(sqlite3* db, STR bookname);
		int noPenalty(sqlite3* db, STR username);
		int notReachBorrowLimit(sqlite3* db, STR username);
		int isBorrowable(sqlite3* db, STR bookid);
		int avalible(sqlite3* db, STR username, STR bookid);
		int isBorrowedBy(sqlite3* db, STR bookid);
		int isBorrowedByUser(sqlite3* db, STR bookid, STR username);
		int isRenewed(sqlite3* db, STR bookid);

		STR searchForBook(sqlite3* db, STR bookisbn);

		void logInChecker(sqlite3*, FILE*);
		void logOutChecker(sqlite3*, FILE*);	
		void setPasswordChecker(sqlite3*, FILE*); 
		void createUserChecker(sqlite3*, FILE*); 
		void removeUserChecker(sqlite3*, FILE*); 
		void createBookChecker(sqlite3*, FILE*); 
		void removeBookChecker(sqlite3*, FILE*);
		void borrowBookChecker(sqlite3*, FILE*);
		void returnBookChecker(sqlite3*, FILE*);
		void renewBookChecker(sqlite3*, FILE*);
		void reserveBookChecker(sqlite3*, FILE*);
		void timeChecker(sqlite3*, FILE*);
		void listAllUsersChecker(sqlite3*, FILE*);
		void listAllBooksChecker(sqlite3*, FILE*);
		void listUserBorrowedBooksChecker(sqlite3*, FILE*);
		void listUserReversedBooksChecker(sqlite3*, FILE*);
		void listBooksByIsbnChecker(sqlite3*, FILE*);
		void listBooksByNameChecker(sqlite3*, FILE*);
		void listBooksByAuthorChecker(sqlite3*, FILE*);
		void listBooksByIndexChecker(sqlite3*, FILE*);
		char buffer[Max_Buffer_Size];
};

#endif /* Not _Tokenizer_H_ */
