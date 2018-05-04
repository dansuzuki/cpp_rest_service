CC=g++
CFLAGS=-std=c++11
INC=-I ../pistache/include -I include/ -I ${HOME}/local/include
LD_FLAGS=-L ../pistache/build/src/ -L ${HOME}/local/lib64 -lmysqlcppconn -lpistache -lpthread 


all : bin/server

bin/server : obj/rest_server.o obj/mysql_client.o
	${CC} -o bin/server $^ ${INC} ${CFLAGS} ${LD_FLAGS} 

obj/mysql_client.o : src/mysql_client.cc include/mysql_client.hpp
	${CC} ${CFLAGS} -o $@ -c src/mysql_client.cc ${INC}
	
obj/rest_server.o : src/rest_server.cc
	${CC} ${CFLAGS} -o $@ -c src/rest_server.cc ${INC}
	
clean :
	rm -rf obj/*.o bin/*