#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>

using namespace std;

#define LOG 1
#define GEET 2
#define ERRROR 3
#define RESPONSE 4

struct logRecord_t
{
	char sensor_id[32];	   // supondo um ID de sensor de até 32 caracteres
	time_t timestamp;      // timestamp UNIX
	double value;		   // valor da leitura
};

struct message_t{
    int type;
	char datetime[100];
	char sensor_id[32];
	int number_records;
	double reading;
};

#endif // COMMON_HPP