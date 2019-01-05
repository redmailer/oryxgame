#include <iostream>
#include "ServerHandler.h"

using namespace std;

int main(int argc, char *argv[]) {
	cout << "Oryx Game begin " << endl;

	if (SINGLETON_INIT(ServerHandler) == false) {
		cout << "Oryx Game failed " << endl;
		return 0;
	}
	
	ServerHandler::getInstance()->run();

	return 0;
}