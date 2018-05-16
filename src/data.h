#ifndef __data_h__
#define __data_h__

#include <string>
#include <unordered_map>

struct User {
	std::string uid;
	std::string uname;
	std::string psd;
	std::string session;
	int status;			//0 离线 1 在线

public:
	User() {
		status = -1;
	}

	void restore() {
		status = -1;
		uid.clear();
		uname.clear();
		psd.clear();
		session.clear();
	}
};

extern std::unordered_map<std::string, User> users;

#endif	//__data_h__
