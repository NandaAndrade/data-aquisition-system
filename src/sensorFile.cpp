#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "../include/sensorFile.hpp"

using namespace std;

time_t Sensor_File::string_to_time_t(const string& time_string) {
	tm tm = {};
	istringstream ss(time_string);
	ss >> get_time(&tm, "%Y-%m-%dT%H:%M:%S");
	return mktime(&tm);
}

string Sensor_File::time_t_to_string(time_t time) {
    tm* tm = localtime(&time);
    ostringstream ss;
    ss << put_time(tm, "%Y-%m-%dT%H:%M:%S");
    return ss.str();
}

logRecord Sensor_File::message_to_log_record(const message_t& msg) {
	logRecord log_record;

	strcpy(log_record.sensor_id, msg.sensor_id);
	log_record.timestamp = string_to_time_t(msg.datetime);
	log_record.value = stod(msg.reading);

	return log_record;
}

// int main()
// {
// 	// Abre o arquivo para leitura e escrita em modo binário e coloca o apontador do arquivo
// 	// apontando para o fim de arquivo
// 	// std::fstream file(".dat", std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::app);
// 	// Caso não ocorram erros na abertura do arquivo
// 	if (file.is_open())
// 	{
// 		// Imprime a posição atual do apontador do arquivo (representa o tamanho do arquivo)
// 		int file_size = file.tellg();

// 		// Recupera o número de registros presentes no arquivo
// 		int n = file_size / sizeof(message_t);
// 		std::cout << "Num records: " << n << " (file size: " << file_size << " bytes)" << std::endl;

// 		// Escreve 10 registros no arquivo
// 		std::cout << "Writing 10 more records..." << std::endl;
// 		int id = n + 1;
// 		for (unsigned i = 0; i < 10; ++i)
// 		{
// 			message_t rec;
// 			rec.id = id++;
// 			gen_random_name(rec.name);
// 			gen_random_phone(rec.phone);
// 			file.write((char *)&rec, sizeof(message_t));
// 		}

// 		// Imprime a posição atual do apontador do arquivo (representa o tamanho do arquivo)
// 		file_size = file.tellg();
// 		// Recupera o número de registros presentes no arquivo
// 		n = file_size / sizeof(message_t);
// 		std::cout << "Num records: " << n << " (file size: " << file_size << " bytes)" << std::endl;

// 		bool id_ok = false;
// 		while (!id_ok)
// 		{
// 			std::cout << "Id: ";
// 			std::cin >> id;
// 			if (id > n)
// 				std::cout << "Invalid id" << std::endl;
// 			else
// 				id_ok = true;
// 		}
// 		file.seekp((id - 1) * sizeof(message_t), std::ios_base::beg);

// 		// Le o registro selecionado
// 		message_t rec;
// 		file.read((char *)&rec, sizeof(message_t));

// 		// Imprime o registro
// 		std::cout << "Id: " << rec.id << " - Name: " << rec.name << " - Phone: " << rec.phone << std::endl;

// 		// Fecha o arquivo
// 		file.close();
// 	}
// 	else
// 	{
// 		std::cout << "Error opening file!" << std::endl;
// 	}
// 	return (0);
// }

int Sensor_File::write_file( logRecord *logRecord)
{
	string file_name = strcat(logRecord->sensor_id, ".dat");
	std::fstream file(file_name, std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::app);
	if (file.is_open())
	{
		file << logRecord << endl;
		// file.write((char *)&msg->content, );
		file.close();
		return 0;
	}
	else
	{
		std::cout << "Error opening file!" << std::endl;
	}
	return 1;
}

logRecord *Sensor_File::read_file(int number_records, char *sensor_id)
{
	string file_name = strcat(sensor_id, ".dat");
	std::fstream file(file_name, std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::app);
	if (file.is_open())
	{
		logRecord *records = new logRecord[number_records];
		file.seekg(0, std::ios::end); // posiciona o apontador no final do arquivo

		for (int i = 0; i < number_records; i++)
		{
			//getline(file, registers[i]);
			file.read((char *)&records[i], sizeof(logRecord));

			cout<<"[read_file] sensor_id: " << records[i].sensor_id
				<<", timestamp: " << time_t_to_string(records[i].timestamp)
				<<", value: " << records[i].value<<endl;

			file.seekg(-1 * (sizeof(message_t)), std::ios::cur);
			
		}
		file.close();
	}
	else
	{
		std::cout << "Error opening file!" << std::endl;
		return;
	}
	
	file.close();
}
