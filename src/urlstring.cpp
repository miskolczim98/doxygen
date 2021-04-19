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
  std::string loc_url = url.str();

  ParseURL(loc_url);
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
  QCString url = (*this)();

  return url.data();
}

char* URLString::rawData()
{
  std::string result = (*this)();

  char* y = new char[result.length() + 1];

  std::strcpy(y, result.c_str());
  return y;
}

std::string URLString::left(int s) const
{
  return (*this)().left(s);
}

std::string URLString::mid(uint index, uint len) const
{
  return (*this)().mid(index, len);
}

std::string URLString::right(int s) const
{
  return (*this)().right(s);
}

std::string URLString::lower() const
{
  return ((QCString)(*this)()).lower().str();
}

char& URLString::at(uint location)
{
  std::string res = (*this)().str();

  char* y;
  std::strcpy(y, res.c_str());

  return *y;
}

const char& URLString::at(uint location) const
{
  std::string& res = (*this)().str();

  return res.at(location);
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

bool URLString::operator==(const URLString& url) const
{
  return (*this)() == url();
}

bool URLString::isEmpty() const
{
  return _protocol.empty() && _userinfo.empty() && _host.empty() &&
    _port.empty() && _path.empty() && _query.empty() && _fragment.empty();
}

bool URLString::startsWith(const std::string& s) const
{
  return (*this)().startsWith(s.c_str());
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
  size_t authority_slashes_begin = GetProtocol().size();

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
  size_t path_begin = static_cast<size_t>(GetProtocol().size()) + GetAuthority().size();

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
  size_t query_begin = static_cast<size_t>(GetProtocol().size()) + GetAuthority().size() + GetPath().size();

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
  size_t fragment_begin = static_cast<size_t>(GetProtocol().size()) + GetAuthority().size() + GetPath().size() + GetQuery().size();

  _fragment = fragment_begin == loc_url.size() || loc_url.substr(fragment_begin, 1) != "#"
    ? ""
    : loc_url.substr(fragment_begin + 1, loc_url.size() - (fragment_begin + 1));
}

std::string URLString::GetProtocol() const
{
  return _protocol.empty() ? "" : _protocol + ":";
}

std::string URLString::GetAuthority() const
{
  if (_host.empty())
    return "";

  std::string userinfo = !_userinfo.empty() ? _userinfo + "@" : "";
  std::string port = !_port.empty() ? ":" + _port : "";

  return "//" + userinfo + _host + port;
}

std::string URLString::GetPath() const
{
  return _path;
}

std::string URLString::GetQuery() const
{
  return _query.empty() ? "" : "?" + _query;
}

std::string URLString::GetFragment() const
{
  return _fragment.empty() ? "" : "#" + _fragment;
}

QCString URLString::operator()() const
{
  return GetProtocol() + GetAuthority() + GetPath() + GetQuery() + GetFragment();
}

bool URLString::CheckAuthorityValidCharacters(const std::string& loc_authority)
{
  return (std::count(loc_authority.begin(), loc_authority.end(), '/') == 0
    && std::count(loc_authority.begin(), loc_authority.end(), '@') <= 1
    && std::count(loc_authority.begin(), loc_authority.end(), ':') <= 2
    && std::count(loc_authority.begin(), loc_authority.end(), '#') == 0
    && std::count(loc_authority.begin(), loc_authority.end(), '?') == 0);
}