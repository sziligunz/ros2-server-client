#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/add_two_ints.hpp"

#include <chrono>
#include <cstdlib>
#include <memory>

#define REQUEST_TIMEOUT 1

using namespace std::chrono_literals;

class Client : public rclcpp::Node {

	long long int a = 0;
	long long int b = 0;

	rclcpp::Client<example_interfaces::srv::AddTwoInts>::SharedPtr client;

	rclcpp::TimerBase::SharedPtr timer;

	using ResponseType = rclcpp::Client<example_interfaces::srv::AddTwoInts>::SharedFuture;
	std::function<void(ResponseType)> handleResponse = [](ResponseType response) {
		if (response.valid())
		{
			RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Sum: %ld", response.get()->sum);
		}
		else
		{
			RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Failed to call service add_two_ints");
		}
	};

	void Loop() {
		if (!this->client->wait_for_service(0s))
		{
			RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "service not available, waiting again...");
			return;
		}

		auto request = std::make_shared<example_interfaces::srv::AddTwoInts::Request>();
		request->a = this->a++;
		request->b = this->b++;

		auto result = client->async_send_request(request, handleResponse);
	}
	
public:

	Client() : Node("client_node") {
		this->client = this->create_client<example_interfaces::srv::AddTwoInts>("service");
		this->timer = create_wall_timer(std::chrono::milliseconds(REQUEST_TIMEOUT), [&]() {Loop();});
	}
};

int main(int argc, char **argv)
{
	rclcpp::init(argc, argv);
	rclcpp::spin(std::make_shared<Client>());
	rclcpp::shutdown();
	return 0;
}