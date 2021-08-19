#include "cloud.hpp"

int main()
{
	cloud_sys::Client client("42.192.83.143", 9000);
	client.Start();
	return 0;
}