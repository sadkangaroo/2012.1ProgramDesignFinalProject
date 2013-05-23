main:Library.o sqlite3.o LibraryDatabaseCreater.o LibOperation.a 
	g++ -o main Library.o sqlite3.o LibraryDatabaseCreater.o LibOperation.a -ldl -lpthread
LibOperation.a:Tokenizer.o Authentication.o Admin.o Reader.o Date.o Query.o
	ar crv LibOperation.a Tokenizer.o Authentication.o Admin.o Reader.o Date.o Query.o
Tokenizer.o:./include/Tokenizer.h ./src/Tokenizer.cpp ./include/sqlite3.h
	g++ -c -I ./include ./src/Tokenizer.cpp
Authentication.o:./include/Tokenizer.h ./src/Authentication.cpp ./include/sqlite3.h
	g++ -c -I ./include ./src/Authentication.cpp
Admin.o:./include/Tokenizer.h ./src/Admin.cpp ./include/sqlite3.h
	g++ -c -I ./include ./src/Admin.cpp
Reader.o:./include/Tokenizer.h ./src/Reader.cpp ./include/sqlite3.h
	g++ -c -I ./include ./src/Reader.cpp
Query.o:./include/Tokenizer.h ./src/Query.cpp ./include/sqlite3.h
	g++ -c -I ./include ./src/Query.cpp
Date.o:./include/Tokenizer.h ./src/Date.cpp ./include/sqlite3.h
	g++ -c -I ./include ./src/Date.cpp
LibraryDatabaseCreater.o:./include/LibraryDatabaseCreater.h ./src/LibraryDatabaseCreater.cpp ./include/sqlite3.h
	g++ -c -I ./include ./src/LibraryDatabaseCreater.cpp
sqlite3.o:./src/sqlite3.c
	gcc -c -I ./include ./src/sqlite3.c
Library.o:./src/Library.cpp ./include/sqlite3.h ./include/LibraryDatabaseCreater.h ./include/Tokenizer.h
	g++ -c -I ./include ./src/Library.cpp
clean:
	rm -f *.o *.a
