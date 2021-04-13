#pragma once

#include "qcstring.h"
#include "textstream.h"

class URLString
{
public:
  URLString();
  URLString(const std::string& url);
  URLString(const QCString& url);
  URLString(const char* url);

  friend TextStream& operator<<(TextStream& t, const URLString& u);
  friend std::ostream& operator<< (std::ostream& stream, const URLString& url);

  std::string operator()() const;
  std::string left(int s) const;
  std::string mid(uint index, uint len = 0xffffffff) const;
  std::string right(int s) const;
  std::string lower() const;

  char& at(uint location) const;

  size_t length() const;

  int find(const char* str) const;
  int find(const char str) const;

  URLString stripWhiteSpace() const;
  URLString& prepend(const QCString& str);
  URLString& operator+= (const QCString& str);

  bool operator==(const URLString& url) const;
  bool URLString::isEmpty() const;

private:
  std::string GetProtocol() const;
  std::string GetAuthority() const;
  std::string GetPath() const;
  std::string GetQuery() const;
  std::string GetFragment() const;

  void ParseURL(const std::string& url);
  void ParseProtocol(const std::string& url);
  void ParseAuthority(const std::string& url);
  void ParseUserInfo(const std::string& authority);
  void ParseHost(const std::string& authority);
  void ParsePort(const std::string& authority);
  void ParsePath(const std::string& authority);
  void ParseQuery(const std::string& authority);
  void ParseFragment(const std::string& authority);

  bool CheckAuthorityValidCharacters(const std::string& authority);

  std::string _protocol;
  std::string _userinfo;
  std::string _host;
  std::string _port;
  std::string _path;
  std::string _query;
  std::string _fragment;
};