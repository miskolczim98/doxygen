#pragma once

#include "qcstring.h"
#include "textstream.h"
#include "qcstring.h"

class URLString
{
public:
  URLString();
  URLString(const std::string& url);
  URLString(const QCString& url);
  URLString(const char* url);

  friend TextStream& operator<<(TextStream& t, const URLString& u);
  friend std::ostream& operator<<(std::ostream& stream, const URLString& url);

  const char* data() const;
  char* rawData();

  QCString left(int s) const;
  QCString mid(uint index, uint len = 0xffffffff) const;
  QCString right(int s) const;
  QCString lower() const;

  QCString operator()() const;

  char& at(uint location);
  const char& at(uint location) const;

  size_t length() const;

  int find(const char* str) const;
  int find(const char str) const;
  int findRev(char c, int index = -1, bool cs = true) const;

  friend int urlcmp(const URLString& url, const char* str2);
  friend int urlcmp(const URLString& url1, const URLString& url2);

  URLString stripWhiteSpace() const;
  URLString& prepend(const QCString& str);
  URLString& operator+= (const QCString& str);

  friend QCString operator+(const char* str, const URLString& url);
  friend QCString operator+(const URLString& url, const char* str);

  bool operator==(const URLString& url) const;
  bool isEmpty() const;
  bool startsWith(const char* s) const;

private:
  QCString GetProtocol() const;
  QCString GetAuthority() const;
  QCString GetPath() const;
  QCString GetQuery() const;
  QCString GetFragment() const;

  size_t GetProtocolSize() const;
  size_t GetAuthoritySize() const;
  size_t GetPathSize() const;
  size_t GetQuerySize() const;
  size_t GetFragmentSize() const;

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