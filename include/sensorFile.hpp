#ifndef SENSORFILE_HPP
#define SENSORFILE_HPP

#include <iostream>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "common.hpp"

using namespace std;

class Sensor_File
{
	public:
		Sensor_File(){};
		~Sensor_File(){};

		time_t string_to_time_t(const string& time_string){	
			tm tm = {};
			istringstream ss(time_string);
			ss >> get_time(&tm, "%Y-%m-%dT%H:%M:%S");
			return mktime(&tm);
		};

		string time_t_to_string(time_t time){
			tm* tm = localtime(&time);
			ostringstream ss;
			ss << put_time(tm, "%Y-%m-%dT%H:%M:%S");
			return ss.str();
		};

		logRecord_t message_to_log_record(const message_t& msg){
			logRecord_t log_record;

			strcpy(log_record.sensor_id, msg.sensor_id);
			log_record.timestamp = string_to_time_t(msg.datetime);
			log_record.value = msg.reading;

			return log_record;
		};

		string createResponse(logRecord_t *logRec, int number_records){
			string response = to_string(number_records);
			for (int i = 0; i < number_records; i++) {
				response += ";" + time_t_to_string(logRec[i].timestamp) + "|" + to_string(logRec[i].value);
			}
			response += "\r\n";
			return response;
		};

		int write_file(logRecord_t *logRec){
			string file_name = strcat(logRec->sensor_id, ".dat");
			fstream file(file_name, fstream::out | fstream::in | fstream::binary | fstream::app);
			if (file.is_open())
			{
				file << logRec << endl;
				// file.write((char *)&msg->content, );
				file.close();
				return 0;
			}
			else
			{
				cout << "Error opening file!" << endl;
			}
			return 1;
		};
		int read_file(int number_records, char *sensor_id, logRecord_t records[]){
			string file_name = strcat(sensor_id, ".dat");
			std::fstream file(file_name, std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::app);
			if (file.is_open())
			{
				
				file.seekg(0, std::ios::end); // posiciona o apontador no final do arquivo

				int i = 0;

				while(file.read((char *)&records[i], sizeof(logRecord_t) && i < number_records)){
					cout<<"[read_file] sensor_id: " << records[i].sensor_id
						<<", timestamp: " << time_t_to_string(records[i].timestamp)
						<<", value: " << records[i].value<<endl;

					file.seekg(-1 * (sizeof(message_t)), std::ios::cur);
					i++;
				}

				cout<<"[read_file] number_returned: " << i <<endl;

				file.close();
				return i;
			}
			else
			{
				std::cout << "Error opening file!" << std::endl;
				return -1;
			}
		};

};

#endif // SENSORFILE_HPP