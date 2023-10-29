#ifndef SENSORFILE_HPP
#define SENSORFILE_HPP

#include <iostream>
#include <cstring>
#include <ctime>

using namespace std;

#define LOG 1
#define GET 2
#define ERROR 3
#define RESPONSE 4

#pragma pack(push, 1)
struct logRecord
{
	char sensor_id[32];	   // supondo um ID de sensor de até 32 caracteres
	time_t timestamp;      // timestamp UNIX
	double value;		   // valor da leitura
};
#pragma pack(pop)

struct message_t{
    int type;
	int id;
	char *content; // guarda todo o conteudo da mensagem
	string datetime;
	char *sensor_id;
	string reading;
};

class Sensor_File
{
public:
    Sensor_File(){};
    ~Sensor_File(){};

    time_t string_to_time_t(const string& time_string);
    string time_t_to_string(time_t time);
	logRecord message_to_log_record(const message_t& msg);
	int write_file(logRecord *logRecord);
	logRecord *read_file(int number_msgs, char *sensor_id);

};

#endif // SENSORFILE_HPP