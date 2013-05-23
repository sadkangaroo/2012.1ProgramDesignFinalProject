#include "Tokenizer.h"
#include "sqlite3.h"

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cctype>
#include<cstring>
#include<string>
#include<sstream>

void Tokenizer::timeChecker(sqlite3* db, FILE* fout) {
	if (strstr(buffer, "get-time")) {
		int sec = atoi(currentTime.c_str());
		time_t tt(sec);
		struct tm* ptm = localtime(&tt);
		fprintf(fout, "get-time\tsuccessful\t%04d/%02d/%02d\n", 1900 + ptm->tm_year, 1 + ptm->tm_mon, ptm->tm_mday);
	}
	if (strstr(buffer, "set-time")) {
		firstToken;
		STR strtime(nextToken);
		int year = atoi(strtime.substr(0, 4).c_str());
		int month = atoi(strtime.substr(5, 2).c_str());
		int day = atoi(strtime.substr(8, 2).c_str());
		struct tm curtm;
		curtm.tm_year = year - 1900;
		curtm.tm_mon = month - 1;
		curtm.tm_mday = day;
		curtm.tm_hour = curtm.tm_min = curtm.tm_sec = 0;
		curtm.tm_isdst = 0;
		int tmp = (int)mktime(&curtm);
		std::stringstream ss;
		ss << tmp;
		ss >> currentTime;
		fprintf(fout, "set-time\tsuccessful\t%04d/%02d/%02d\n", year, month, day);
	}
}
