http_srv:http_srv.cpp
	g++ -std=c++11 $^ -o $@ -lpthread
mysql_test:mysql.c
	gcc $^ -o $@ -L/usr/lib64/mysql -lmysqlclient
json_test:json.cpp
	  g++ -std=c++11 $^ -o $@ -ljsoncpp
