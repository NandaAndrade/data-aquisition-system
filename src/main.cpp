#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

#include "../include/sensorFile.hpp"
#include "../include/common.hpp"
#include "../include/session.hpp"

#define LOG 1
#define GET 2
#define ERROR 3
#define RESPONSE 4

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
