#include "Tokenizer.h"
#include "sqlite3.h"

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cctype>
#include<cstring>
#include<string>

STR currentTime;

int loadLogInAuthentication(void* password, int cnum, char** cvalue, char** cname) {
	if (strcmp(((STR*)password)->c_str(), cvalue[1]) == 0) *((STR*)password) = "changedsuccessfully";
	return 0;
}

int loadLogUserInfo(void* flag, int cnum, char** cvalue, char** cname) {
	if (strcmp(cvalue[0], "login") == 0)  *((int*)flag) = 1;
	return 0;
}

int loadExistInfo(void* flag, int cnum, char** cvalue, char** cname) {
	*((int*)flag) = 1;
	return 0;
}

int loadAdminInfo(void* flag, int cnum, char** cvalue, char** cname) {
	if (strcmp(cvalue[0], "login") == 0 && strcmp(cvalue[1], "ADMIN") == 0) *((int*)flag) = 1;
	return 0;
}

int loadReaderInfo(void* flag, int cnum, char** cvalue, char** cname) {
	if (strcmp(cvalue[0], "login") == 0 && strcmp(cvalue[1], "READER") == 0) *((int*)flag) = 1;
	return 0;
}

int loadTeacherInfo(void* flag, int cnum, char** cvalue, char** cname) {
	if (strcmp(cvalue[0], "TEACHER") == 0) *((int*)flag) = 1;
	return 0;
}

int loadPenaltyInfo(void* flag, int cnum, char** cvalue, char** cname) {
	int t1 = atoi(cvalue[0]);
	int t2 = atoi(currentTime.c_str());
	if (t2 - t1 >= BorrowedBookExpiredSec) *((int*)flag) = 1;
	return 0;
}

int loadReacherInfoTeacher(void* flag, int cnum, char** cvalue, char** cname) {
	if (*((int*)flag) == -1) return 0;
	(*((int*)flag))++;
	if (*((int*)flag) == TeacherBorrowLimit) *((int*)flag) = -1;
	return 0;
}

int loadReacherInfoStudent(void* flag, int cnum, char** cvalue, char** cname) {
	if (*((int*)flag) == -1) return 0;
	(*((int*)flag))++;
	if (*((int*)flag) == StudentBorrowLimit) *((int*)flag) = -1;
	return 0;
}

int loadBorrowableInfo(void* flag, int cnum, char** cvalue, char** cname) {
	if (strcmp(cvalue[0], "true") == 0) *((int*)flag) = 1;
	return 0;
}

int loadAvalibleInfo(void* username, int cnum, char** cvalue, char** cname) {
	if (*((STR*)username) == "") return 0;
	if (strcmp(cvalue[0], "null") != 0) *((STR*)username) = "";
	if (strcmp(cvalue[1], "null") != 0 && strcmp(cvalue[1], ((STR*)username)->c_str()) != 0) {
		int t1 = atoi(cvalue[2]);
		int t2 = atoi(currentTime.c_str());
		if (t2 - t1 < ReserveBookExpireSec) *((STR*)username) = "";
	}
	return 0; 
} 

int loadBorrowedByInfo(void* flag, int cnum, char** cvalue, char** cname) { 
	if (strcmp(cvalue[0], "null") != 0) *((int*)flag) = 1; 
	return 0; 
} 

int loadBorrowedByUserInfo(void* username, int cnum, char** cvalue, char** cname) { 
	if (strcmp(cvalue[0], ((STR*)username)->c_str()) != 0) *((STR*)username) = ""; 
	return 0; 
} 

int loadRenewedInfo(void* flag, int cnum, char** cvalue, char** cname) { 
	if (strcmp(cvalue[0], "false") != 0) *((int*)flag) = 1;
	return 0;
}

int loadIsbnInfo(void* bookid, int cnum, char** cvalue, char** cname) {
	if ((strcmp(cvalue[1], "null") == 0 || atoi(currentTime.c_str()) - atoi(cvalue[2]) >= ReserveBookExpireSec)) *((STR*)bookid) = STR(cvalue[3]);
	return 0;
}

bool Tokenizer::load(FILE* in) {
	return fgets(buffer, Max_Buffer_Size, in) != NULL; 
}

STR Tokenizer::searchForBook(sqlite3* db, STR bookisbn) {
	STR bookid = "";
	STR command = "select borrowedby,reservedby,reservedsince,id from Book where isbn='" + bookisbn + "'";
	sqlite3_exec(db, command.c_str(), loadIsbnInfo, &bookid, NULL);
	return bookid;
}

int Tokenizer::isLogIn(sqlite3* db, STR username) {
	int isLogRet = 0;
	STR command = "select status from User where name='" + username + "'";
	sqlite3_exec(db, command.c_str(), loadLogUserInfo, &isLogRet, NULL);
	return isLogRet;
}

int Tokenizer::isLogInAdmin(sqlite3* db, STR adminname) {
	int isLogAdminRet = 0;
	STR command = "select status,type from User where name='" + adminname + "'";
	sqlite3_exec(db, command.c_str(), loadAdminInfo, &isLogAdminRet, NULL);
	return isLogAdminRet;
}

int Tokenizer::isLogInReader(sqlite3* db, STR username) {
	int isLogReaderRet = 0;
	STR command = "select status,generaltype from User where name='" + username + "'";
	sqlite3_exec(db, command.c_str(), loadReaderInfo, &isLogReaderRet, NULL);
	return isLogReaderRet;
}

int Tokenizer::isTeacher(sqlite3* db, STR username) {
	int isTeacherRet = 0;
	STR command = "select type from User where name='" + username + "'";
	sqlite3_exec(db, command.c_str(), loadTeacherInfo, &isTeacherRet, NULL);
	return isTeacherRet;
}

int Tokenizer::noPenalty(sqlite3* db, STR username) {
	int hasPenaltyRet = 0;
	STR command = "select borrowedsince from Book where borrowedby='" + username + "'";
	sqlite3_exec(db, command.c_str(), loadPenaltyInfo, &hasPenaltyRet, NULL);
	return !hasPenaltyRet;
}

int Tokenizer::notReachBorrowLimit(sqlite3* db, STR username) {
	int reachBorrowLimitRet = 0;
	STR command = "select borrowedby from Book where borrowedby='" + username + "'";
	if (isTeacher(db, username)) 
		sqlite3_exec(db, command.c_str(), loadReacherInfoTeacher, &reachBorrowLimitRet, NULL);
	else sqlite3_exec(db, command.c_str(), loadReacherInfoStudent, &reachBorrowLimitRet, NULL);
	return !(reachBorrowLimitRet == -1);
}

int Tokenizer::isBorrowable(sqlite3* db, STR bookid) {
	int borrowableRet = 0;
	STR command = "select borrowable from Book where id='" + bookid + "'";
	sqlite3_exec(db, command.c_str(), loadBorrowableInfo, &borrowableRet, NULL);
	return borrowableRet;
}

int Tokenizer::avalible(sqlite3* db, STR username, STR bookid) {
	STR command = "select borrowedby,reservedby,reservedsince from Book where id='" + bookid + "'";
	sqlite3_exec(db, command.c_str(), loadAvalibleInfo, &username, NULL);
	return username != "";
}

int Tokenizer::isBorrowedBy(sqlite3* db, STR bookid) {
	int borrowedByRet = 0;
	STR command = "select borrowedby from Book where id='" + bookid + "'";
	sqlite3_exec(db, command.c_str(), loadBorrowedByInfo, &borrowedByRet, NULL);
	return borrowedByRet;
}

int Tokenizer::isBorrowedByUser(sqlite3* db, STR bookid, STR username) {
	STR command = "select borrowedby from Book where id='" + bookid + "'";
	sqlite3_exec(db, command.c_str(), loadBorrowedByUserInfo, &username, NULL);
	return username != "";
}

int Tokenizer::isRenewed(sqlite3* db, STR bookid) {
	int renewedRet = 0;
	STR command = "select isrenewed from Book where id='" + bookid + "'";
	sqlite3_exec(db, command.c_str(), loadRenewedInfo, &renewedRet, NULL);
	return renewedRet;
}

int Tokenizer::userExist(sqlite3* db, STR username) {
	int userExRet = 0;
	STR command = "select name from User where name='" + username + "'";
	sqlite3_exec(db, command.c_str(), loadExistInfo, &userExRet, NULL);
	return userExRet;
}

int Tokenizer::bookExist(sqlite3* db, STR bookid) {
	int bookExRet = 0;
	STR command = "select id from Book where id='" + bookid + "'";
	sqlite3_exec(db, command.c_str(), loadExistInfo, &bookExRet, NULL);
	return bookExRet;
}

int	Tokenizer::getTokens(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "restart") != NULL) return 1;		
	logInChecker(db, fout);
	logOutChecker(db, fout);
	setPasswordChecker(db, fout);
	createUserChecker(db, fout);
	removeUserChecker(db, fout);
	createBookChecker(db, fout);
	removeBookChecker(db, fout);
	borrowBookChecker(db, fout);
	returnBookChecker(db, fout);
	renewBookChecker(db, fout);
	reserveBookChecker(db, fout);
	timeChecker(db, fout);
	listAllUsersChecker(db, fout);
	listAllBooksChecker(db, fout);
	listUserBorrowedBooksChecker(db, fout);
	listUserReversedBooksChecker(db, fout);
	listBooksByIsbnChecker(db, fout);
	listBooksByNameChecker(db, fout);
	listBooksByAuthorChecker(db, fout);
	listBooksByIndexChecker(db, fout);
	return 0;	
}
