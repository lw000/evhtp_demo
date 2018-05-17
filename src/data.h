#ifndef __data_h__
#define __data_h__

#include <string>
#include <vector>
#include <unordered_map>

struct User {
	std::string uid;
	std::string uname;
	std::string psd;
	std::string token;
	int status;		//0 离线 1 在线

public:
	User();
	void restore();

public:
	std::string Serializable();
	void UnSerializable(const std::string& json);
};

extern std::vector<User> users_table;

extern std::unordered_map<std::string, User> users;


#endif	//__data_h__
