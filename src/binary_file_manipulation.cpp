#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>

#pragma pack(push, 1)
struct LogRecord {
    char sensor_id[32]; // supondo um ID de sensor de até 32 caracteres
    std::time_t timestamp; // timestamp UNIX
    double value; // valor da leitura
};
#pragma pack(pop)

using namespace std;
struct message_t
{
	int type;
	int id;
	char *content; // guarda todo o conteudo da mensagem
	string datetime;
	char *sensor_id;
	string reading;
};

// void gen_random_name(char *s, const int len = 15)
// {
// 	static const char alphanum[] =
// 		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
// 		"abcdefghijklmnopqrstuvwxyz";

// 	for (int i = 0; i < len; ++i)
// 	{
// 		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
// 	}
// 	s[len] = 0;
// }

// void gen_random_phone(char *s, const int len = 9)
// {
// 	static const char alphanum[] = "0123456789";

// 	for (int i = 0; i < len; ++i)
// 	{
// 		if (i == 4)
// 			s[i] = '-';
// 		else
// 			s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
// 	}
// 	s[len] = 0;
// }

int main()
{
	// Abre o arquivo para leitura e escrita em modo binário e coloca o apontador do arquivo
	// apontando para o fim de arquivo
	// std::fstream file(".dat", std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::app);
	// Caso não ocorram erros na abertura do arquivo
	if (file.is_open())
	{
		// Imprime a posição atual do apontador do arquivo (representa o tamanho do arquivo)
		int file_size = file.tellg();

		// Recupera o número de registros presentes no arquivo
		int n = file_size / sizeof(message_t);
		std::cout << "Num records: " << n << " (file size: " << file_size << " bytes)" << std::endl;

		// Escreve 10 registros no arquivo
		std::cout << "Writing 10 more records..." << std::endl;
		int id = n + 1;
		for (unsigned i = 0; i < 10; ++i)
		{
			message_t rec;
			rec.id = id++;
			gen_random_name(rec.name);
			gen_random_phone(rec.phone);
			file.write((char *)&rec, sizeof(message_t));
		}

		// Imprime a posição atual do apontador do arquivo (representa o tamanho do arquivo)
		file_size = file.tellg();
		// Recupera o número de registros presentes no arquivo
		n = file_size / sizeof(message_t);
		std::cout << "Num records: " << n << " (file size: " << file_size << " bytes)" << std::endl;

		bool id_ok = false;
		while (!id_ok)
		{
			std::cout << "Id: ";
			std::cin >> id;
			if (id > n)
				std::cout << "Invalid id" << std::endl;
			else
				id_ok = true;
		}
		file.seekp((id - 1) * sizeof(message_t), std::ios_base::beg);

		// Le o registro selecionado
		message_t rec;
		file.read((char *)&rec, sizeof(message_t));

		// Imprime o registro
		std::cout << "Id: " << rec.id << " - Name: " << rec.name << " - Phone: " << rec.phone << std::endl;

		// Fecha o arquivo
		file.close();
	}
	else
	{
		std::cout << "Error opening file!" << std::endl;
	}
	return (0);
}

int write_file(message_t *msg)
{
	string file_name = strcat(msg->sensor_id, ".dat");
	std::fstream file(file_name, std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::app);
	if (file.is_open())
	{
		file << msg->content<<endl;
		//file.write((char *)&msg->content, );
		file.close();
		return 0;
	}
	else
	{
		std::cout << "Error opening file!" << std::endl;
	}
	return 1;
}

message_t *read_file(int number_msgs, char *sensor_id)
{
	string file_name = strcat(sensor_id, ".dat");
	std::fstream file(file_name, std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::app);
	if (file.is_open())
	{
		message_t *list_msg = new message_t[number_msgs];
		file.seekg(0, std::ios::end); // posiciona o apontador no final do arquivo

		for (int i = 0; i < number_msgs; i++)
		{
			getline(file,list_msg[i].content);
			//file.read(list_msg[i].content, sizeof(char *));
			file.seekg(-1 * (sizeof(message_t)), std::ios::cur);
			cout << list_msg[i].content << endl;
		}
		file.close();
	}
	else
	{
		std::cout << "Error opening file!" << std::endl;
	}
	return;

	// Le o registro selecionado
	message_t rec;
	file.read((char *)&rec, sizeof(message_t));

	// Imprime o registro
	std::cout << "Id: " << rec.id << " - Name: " << rec.name << " - Phone: " << rec.phone << std::endl;

	// Fecha o arquivo
	file.close();
}
else
{
	std::cout << "Error opening file!" << std::endl;
}
return (0);
}