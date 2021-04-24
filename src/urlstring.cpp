#include "urlstring.h"

#include <stdexcept>
#include <string>
#include <algorithm>

URLString::URLString()
{
  _protocol = _userinfo = _host = _port = _path =
    _query = _fragment = "";
}

URLString::URLString(const std::string& url)
{
  ParseURL(url);
}

URLString::URLString(const QCString& url)
{
  ParseURL(url.str());
}

URLString::URLString(const char* url)
{
  ParseURL(url);
}

TextStream& operator<<(TextStream& t, const URLString& u)
{
  t << u();
  return t;
}

std::ostream& operator<<(std::ostream& stream, const URLString& url)
{
  stream << url();
  return stream;
}

URLString URLString::stripWhiteSpace() const
{
  return URLString((*this)().stripWhiteSpace());
}

const char* URLString::data() const
{
  return (*this)().data();
}

char* URLString::rawData()
{
  QCString result = (*this)();

  char* y = new char[static_cast<size_t>(result.length()) + 1];

  std::strcpy(y, result);
  return y;
}

QCString URLString::left(int s) const
{
  return (*this)().left(s);
}

QCString URLString::mid(uint index, uint len) const
{
  return (*this)().mid(index, len);
}

QCString URLString::right(int s) const
{
  return (*this)().right(s);
}

QCString URLString::lower() const
{
  return ((*this)()).lower();
}

char& URLString::at(uint location)
{
  QCString result = (*this)();

  char* y = new char[static_cast<size_t>(result.length()) + 1];
  std::strcpy(y, result);

  return *(y + location);
}

const char& URLString::at(uint location) const
{
  QCString result = (*this)().str();

  char* y = new char[static_cast<size_t>(result.length()) + 1];
  std::strcpy(y, result);

  return *(y + location);
}

size_t URLString::length() const
{
  return (*this)().size();
}

int URLString::find(const char* str) const
{
  return (*this)().find(str);
}

int URLString::find(const char c) const
{
  return (*this)().find(c);
}

int URLString::findRev(char c, int index, bool cs) const
{
  return (*this)().findRev(c, index, cs);
}

int urlcmp(const URLString& url, const char* str2)
{
  return qstrcmp(url(), str2);
}

int urlcmp(const URLString& url1, const URLString& url2)
{
  return qstrcmp(url1(), url2());
}

URLString& URLString::prepend(const QCString& str)
{
  *this = URLString(str.str() + (*this)());
  return *this;
}

URLString& URLString::operator+= (const QCString& str)
{
  *this = URLString((*this)() + str);
  return *this;
}

QCString operator+(const char* str, const URLString& url)
{
  return url() + str;
}

QCString operator+(const URLString& url, const char* str)
{
  return url() + str;
}

bool URLString::operator==(const URLString& url) const
{
  return (*this)() == url();
}

bool URLString::isEmpty() const
{
  return _protocol.empty() && _userinfo.empty() && _host.empty() &&
    _port.empty() && _path.empty() && _query.empty() && _fragment.empty();
}

bool URLString::startsWith(const char* s) const
{
  return (*this)().startsWith(s);
}

void URLString::ParseURL(const std::string& url)
{
  ParseProtocol(url);
  ParseAuthority(url);
  ParsePath(url);
  ParseQuery(url);
  ParseFragment(url);
}

void URLString::ParseProtocol(const std::string& loc_url)
{
  size_t comma_location = loc_url.find(":");

  _protocol = comma_location != std::string::npos
    ? loc_url.substr(0, comma_location)
    : "";
}

void URLString::ParseAuthority(const std::string& loc_url)
{
  size_t authority_slashes_begin = GetProtocolSize();

  if (authority_slashes_begin < loc_url.size() && loc_url.substr(authority_slashes_begin, 2) == "//")
  {
    size_t authority_begin_location = authority_slashes_begin + 2;

    size_t authority_end_location = loc_url
      .substr(authority_begin_location, loc_url.size() - authority_begin_location).find("/");

    authority_end_location = authority_end_location != std::string::npos
      ? authority_begin_location + authority_end_location
      : loc_url.size();

    std::string authority = loc_url
      .substr(authority_begin_location, authority_end_location - authority_begin_location);

    if (!CheckAuthorityValidCharacters(authority))
      throw std::invalid_argument("Invalid URL: Invalid characters in authority.");

    ParseUserInfo(authority);
    ParseHost(authority);
    ParsePort(authority);
  }
  else
    _userinfo = _host = _port = "";
}

void URLString::ParseUserInfo(const std::string& loc_authority)
{
  size_t userinfo_at_location = loc_authority.find("@");

  if (userinfo_at_location != std::string::npos)
  {
    _userinfo = loc_authority.substr(0, userinfo_at_location);

    if (_userinfo.empty())
      throw std::invalid_argument("Invalid URL: No userinfo added.");
  }
  else
    _userinfo = "";
}

void URLString::ParseHost(const std::string& loc_authority)
{
  size_t host_start = !_userinfo.empty() ? _userinfo.size() + 1 : 0;

  size_t host_end = loc_authority.substr(host_start, loc_authority.size() - host_start).find(":");
  host_end = host_end == std::string::npos ? host_end = loc_authority.size() : host_end + host_start;

  if (host_start == host_end)
    throw std::invalid_argument("Invalid URL: No host added.");

  _host = loc_authority.substr(host_start, host_end - host_start);
}

void URLString::ParsePort(const std::string& loc_authority)
{
  size_t port_start = !_userinfo.empty() ? _userinfo.size() + _host.size() + 1 : _host.size();

  if (port_start < loc_authority.size() && loc_authority.substr(port_start, 1) == ":")
  {
    ++port_start;

    if (port_start == loc_authority.size())
      throw std::invalid_argument("Invalid URL: No port added.");

    _port = loc_authority.substr(port_start, loc_authority.size() - port_start);
  }
  else
    _port = "";
}

void URLString::ParsePath(const std::string& loc_url)
{
  size_t path_begin = GetProtocolSize() + GetAuthoritySize();

  size_t first_query_loc = loc_url.substr(path_begin, loc_url.size() - (path_begin)).find("?");

  if (first_query_loc != std::string::npos)
  {
    first_query_loc += path_begin;
    _path = first_query_loc < path_begin ? "" : loc_url.substr(path_begin, first_query_loc - path_begin);

    return;
  }

  size_t first_fragment_loc = loc_url.substr(path_begin, loc_url.size() - path_begin).find("#");

  if (first_fragment_loc != std::string::npos)
  {
    first_fragment_loc += path_begin;
    _path = first_fragment_loc < path_begin ? "" : loc_url.substr(path_begin, first_fragment_loc - path_begin);

    return;
  }

  _path = loc_url.substr(path_begin, loc_url.size() - path_begin);
}

void URLString::ParseQuery(const std::string& loc_url)
{
  size_t query_begin = GetProtocolSize() + GetAuthoritySize() + GetPathSize();

  if (loc_url.size() == query_begin || loc_url.substr(query_begin, 1) != "?")
  {
    _query = "";
    return;
  }

  ++query_begin;

  size_t first_fragment_loc = loc_url.substr(query_begin, loc_url.size() - query_begin).find("#");

  if (first_fragment_loc != std::string::npos)
  {
    first_fragment_loc += query_begin;
    _query = loc_url.substr(query_begin, first_fragment_loc - query_begin);
  }
  else
  {
    _query = loc_url.substr(query_begin, loc_url.size() - query_begin);
  }
}

void URLString::ParseFragment(const std::string& loc_url)
{
  size_t fragment_begin = GetProtocolSize() + GetAuthoritySize() + GetPathSize() + GetQuerySize();

  _fragment = fragment_begin == loc_url.size() || loc_url.substr(fragment_begin, 1) != "#"
    ? ""
    : loc_url.substr(fragment_begin + 1, loc_url.size() - (fragment_begin + 1));
}

QCString URLString::GetProtocol() const
{
  return _protocol.empty() ? "" : _protocol + ":";
}

QCString URLString::GetAuthority() const
{
  if (_host.empty())
    return "";

  std::string userinfo = !_userinfo.empty() ? _userinfo + "@" : "";
  std::string port = !_port.empty() ? ":" + _port : "";

  return "//" + userinfo + _host + port;
}

QCString URLString::GetPath() const
{
  return _path;
}

QCString URLString::GetQuery() const
{
  return _query.empty() ? "" : "?" + _query;
}

QCString URLString::GetFragment() const
{
  return _fragment.empty() ? "" : "#" + _fragment;
}

QCString URLString::operator()() const
{
  return GetProtocol() + GetAuthority() + GetPath() + GetQuery() + GetFragment();
}

size_t URLString::GetProtocolSize() const
{
  return _protocol.empty() ? 0 : _protocol.size() + 1;
}

size_t URLString::GetAuthoritySize() const
{
  if (_host.empty())
    return 0;

  size_t userinfoSize = !_userinfo.empty() ? _userinfo.size() + 1 : 0;
  size_t portSize = !_port.empty() ? 1 + _port.size() : 0;

  return 2 + userinfoSize + _host.size() + portSize;
}

size_t URLString::GetPathSize() const
{
  return _path.size();
}

size_t URLString::GetQuerySize() const
{
  return _query.empty() ? 0 : 1 + _query.size();
}

size_t URLString::GetFragmentSize() const
{
  return _fragment.empty() ? 0 : 1 + _fragment.size();
}

bool URLString::CheckAuthorityValidCharacters(const std::string& loc_authority)
{
  return (std::count(loc_authority.begin(), loc_authority.end(), '/') == 0
    && std::count(loc_authority.begin(), loc_authority.end(), '@') <= 1
    && std::count(loc_authority.begin(), loc_authority.end(), ':') <= 2
    && std::count(loc_authority.begin(), loc_authority.end(), '#') == 0
    && std::count(loc_authority.begin(), loc_authority.end(), '?') == 0);
}