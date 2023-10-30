#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

#include "../include/sensorFile.hpp"

#define LOG 1
#define GET 2
#define ERROR 3
#define RESPONSE 4

using boost::asio::ip::tcp;
using namespace std;

class session : public std::enable_shared_from_this<session>
{
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
					cout << "Received: " << message << endl;
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
						case GET:
							cout << "[receive_message] GET" << endl;
							number_returned  = sensor_file.read_file(msg.number_records, msg.sensor_id, rec_get);
							response = sensor_file.createResponse(rec_get, number_returned);
							response_message(response);
							break;
						case ERROR:
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
		message_t msg;
		if (message.find("LOG"))
		{
			msg.type = LOG;
			sprintf("LOG|%s|%s|%d\r\n", msg.sensor_id, msg.datetime, msg.reading);
			return msg;
    	}

		if (message.find("GET"))
		{
			msg.type = GET;
			sprintf("GET|%s|%d\r\n", msg.sensor_id, msg.number_records);
			return msg;
		}

		if (message.find("ERROR"))
		{
			msg.type = ERROR;
			sprintf("ERROR|%d\r\n", msg.sensor_id);
			return msg;
		}
	}

private:
	tcp::socket socket_;
	boost::asio::streambuf buffer_;
};

class server
{
public:
	server(boost::asio::io_context &io_context, short port) 
		: acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
	{ 
		accept();
	}

private:
  void accept()
  {
    cout << "[accept] ouvindo em : " << acceptor_.local_endpoint().address().to_string() << endl;
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket)
        {
          if (!ec)
          {
            cout << "[accept] ec: << " << ec << endl;
            std::make_shared<session>(std::move(socket))->start();
          }
          else
          {
            cout << "[accept] falha/!" << endl;
          }

          accept();
        });
  }

  tcp::acceptor acceptor_;
};

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cerr << "Usage: chat_server <port>\n";
    return 1;
  }

  boost::asio::io_context io_context;

  server s(io_context, std::atoi(argv[1]));

  cout << "[main]" << endl;

  io_context.run();

  return 0;
}
