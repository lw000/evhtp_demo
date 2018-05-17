#include "data.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/filestream.h"

std::vector<User> users_table;

std::unordered_map<std::string, User> users;

User::User() {
	status = -1;
}

void User::restore() {
	status = -1;
	uid.clear();
	uname.clear();
	psd.clear();
	token.clear();
}

std::string User::Serializable() {
	rapidjson::Document d;
	d.SetObject();
	rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
	d.AddMember("uid", uid.c_str(), allocator);
	d.AddMember("uname", uname.c_str(), allocator);
	d.AddMember("psd", psd.c_str(), allocator);
	d.AddMember("token", token.c_str(), allocator);
	d.AddMember("status", status, allocator);
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	std::string result = buffer.GetString();

	return result;
}

void User::UnSerializable(const std::string& json) {

}
