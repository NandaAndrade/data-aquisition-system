#ifndef SESSION_HPP
#define SESSION_HPP

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

#include "common.hpp"
#include "sensorFile.hpp"

using boost::asio::ip::tcp;
using namespace std;

class session : public std::enable_shared_from_this<session>
{
    private:
		tcp::socket socket_;
		boost::asio::streambuf buffer_;

	public:
		session(tcp::socket socket): socket_(std::move(socket)){}

		void start(){ receive_message();	}

		void receive_message()
		{
			cout << "[receive_message]" << endl;
			auto self(shared_from_this());
			boost::asio::async_read_until(socket_, buffer_, "\r\n",
				[this, self](boost::system::error_code ec, std::size_t length)
				{
					if (!ec)
					{
						istream is(&buffer_);
						string message(istreambuf_iterator<char>(is), {});
						cout << "[receive_message] Received: " << message << endl;
						message_t msg = discoverSender(message);
						Sensor_File sensor_file;
						logRecord_t rec_log = {};
						logRecord_t rec_get[msg.number_records] = {};
						int number_returned  = 0;
						string response = "";

						switch (msg.type)
						{
							case LOG:
								cout << "[receive_message] LOG" << endl;
								rec_log = sensor_file.message_to_log_record(msg);
								if(sensor_file.write_file(&rec_log))
									cout << "[receive_message] ERROR" << endl;
								cout << "[receive_message] mensagem de Log escrita no arquivo" << endl;
								break;
							case GEET:
								cout << "[receive_message] GET" << endl;
								number_returned  = sensor_file.read_file(msg.number_records, msg.sensor_id, rec_get);
								response = sensor_file.createResponse(rec_get, number_returned);
								response_message(response);
								break;
							case ERRROR:
								cout << "[receive_message] ERROR" << endl;
								break;
							default:
								cout << "[receive_message] ERROR" << endl;
								break;
						}

						
					}
				});
		}

		void response_message(const std::string &message)
		{
			auto self(shared_from_this());
			boost::asio::async_write(socket_, boost::asio::buffer(message),
				[this, self, message](boost::system::error_code ec, std::size_t /*length*/)
				{
				if (!ec)
				{
					receive_message();
				}
			});
		}

		message_t discoverSender(string message)
		{
			cout<<"[discoverSender] message: "<< message <<endl;
			message_t msg;
            int numItems;

			size_t found = message.find("LOG");
			if (found != string::npos)
			{
				cout<<"[discoverSender] LOG"<<endl;
				msg.type = LOG;
				numItems = sscanf(message.c_str(), "LOG|%99[^|]|%99[^|]|%lf", msg.sensor_id, msg.datetime, &msg.reading);

				if (numItems == 3) {
                    cout<<"[discoverSender] sensor_id: "<< msg.sensor_id 
                        << " datetime: " << msg.datetime 
                        << " reading: " << msg.reading 
                        <<endl;
                }
				return msg;
			}

			found = message.find("GET");
			if (found != string::npos)
			{
				msg.type = GEET;
                numItems = sscanf(message.c_str(), "GET|%99[^|]|%lf", msg.sensor_id, &msg.number_records);
                if(numItems == 2){
                    cout<<"[discoverSender] sensor_id: "<< msg.sensor_id 
                        << " number_records: " << msg.number_records 
                        <<endl;
                }
                return msg;
			}

			found = message.find("ERROR");
			if (found != string::npos)
			{
				msg.type = ERRROR;
                numItems = sscanf(message.c_str(), "ERROR|%99[^|]", msg.sensor_id);
                if(numItems == 1){
				    cout<<"[discoverSender] sensor_id: "<< msg.sensor_id <<endl;
                }
				return msg;
			}
		}
};

#endif // SESSION_HPP