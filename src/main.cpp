#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

#define LOG 1
#define GET 2
#define ERROR 3
#define RESPONSE 4

using boost::asio::ip::tcp;
using namespace std;

struct message_t
{
  int type;
  string content;
  string datetime;
  string sensor_id;
  string reading;
};

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
					std::istream is(&buffer_);
					std::string message(std::istreambuf_iterator<char>(is), {});
					std::cout << "Received: " << message << std::endl;
					message_t msg = discoverSender(message);

					switch (msg.type)
					{
						case LOG:
							cout << "[receive_message] LOG" << endl;
							break;
						case GET:
							cout << "[receive_message] GET" << endl;
							break;
						case ERROR:
							cout << "[receive_message] ERROR" << endl;
							break;
						default:
							cout << "[receive_message] ERROR" << endl;
							break;
					}

					response_message(message);
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
			// LOG|SENSOR_ID|DATA_HORA|LEITURA\r\n
			msg.type = LOG;
			msg.content = message;
			//   msg.sensor_id = message.substr(message.find("|") + 1, message.find("|", message.find("|") + 1));
			//   msg.datetime = message.substr(message.find("|") + 1, message.find("|", message.find("|") + 1));
			return msg;
    	}

		if (message.find("GET"))
		{
			msg.type = GET;
			msg.content = message;
			return msg;
		}

		if (message.find("ERROR"))
		{
			msg.type = ERROR;
			msg.content = message;
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
