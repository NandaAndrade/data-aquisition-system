#ifndef SENSORFILE_HPP
#define SENSORFILE_HPP

#include <iostream>
#include <cstring>
#include <ctime>
#include <tuple>

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
	string datetime;
	char *sensor_id;
	int number_records;
	double reading;
};

class Sensor_File
{
public:
    Sensor_File(){};
    ~Sensor_File(){};

    time_t string_to_time_t(const string& time_string);
    string time_t_to_string(time_t time);
	logRecord_t message_to_log_record(const message_t& msg);
	string createResponse(logRecord_t *logRecord, int number_records);
	int write_file(logRecord_t *logRecord);
	int read_file(int number_msgs, char *sensor_id, logRecord_t records[]);

};

#endif // SENSORFILE_HPP