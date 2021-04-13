#pragma once

#include "qcstring.h"

class URLString
{
public:
  URLString(const QCString& url);

  std::string operator()() const;

private:
  std::string GetProtocol() const;
  std::string GetAuthority() const;
  std::string GetPath() const;
  std::string GetQuery() const;
  std::string GetFragment() const;

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