#include "urlstring.h"

#include <stdexcept>
#include <string>
#include <algorithm>

URLString::URLString()
{
  _protocol = _userinfo = _host = _port = _path =
    _query = _fragment = "";
}

URLString::URLString(const QCString& url)
{
  std::string loc_url = url;

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

std::ostream& operator<< (std::ostream& stream, const URLString& url)
{
  stream << url();
  return stream;
}

URLString URLString::stripWhiteSpace() const
{
  std::string result = (*this)();

  for (size_t i = 0, j = 0; result[j] = result[i]; j += !isspace(result[i++]));

  return URLString(result);
}

std::string URLString::left(int s) const
{
  return (*this)().substr(0, s);
}

char& URLString::at(uint location) const
{
  std::string& res = (*this)();

  return res.at(location);
}

URLString& URLString::prepend(const QCString& str)
{
  *this = URLString(str.str() + (*this)());
  return *this;
}

bool URLString::operator==(const URLString& url) const
{
  return (*this)() == url();
}

bool URLString::isEmpty() const
{
  return _protocol == "" && _userinfo == "" && _host == "" &&
    _port == "" && _path == "" && _query == "" && _fragment == "";
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

  if (comma_location == std::string::npos)
    throw std::invalid_argument("Invalid URL: No protocol added.");

  std::string protocol = loc_url.substr(0, comma_location);

  if (protocol == "http" || protocol == "https" ||
    protocol == "ftp" || protocol == "ftps" ||
    protocol == "sftp" || protocol == "file" ||
    protocol == "news" || protocol == "irc" ||
    protocol == "ircs")
    _protocol = protocol;
  else
    _protocol = "";
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

    if (_userinfo == "")
      throw std::invalid_argument("Invalid URL: No userinfo added.");
  }
  else
    _userinfo = "";
}

void URLString::ParseHost(const std::string& loc_authority)
{
  size_t host_start = _userinfo != "" ? _userinfo.size() + 1 : 0;

  size_t host_end = loc_authority.substr(host_start, loc_authority.size() - host_start).find(":");
  host_end = host_end == std::string::npos ? host_end = loc_authority.size() : host_end + host_start;

  if (host_start == host_end)
    throw std::invalid_argument("Invalid URL: No host added.");

  _host = loc_authority.substr(host_start, host_end - host_start);
}

void URLString::ParsePort(const std::string& loc_authority)
{
  size_t port_start = _userinfo != "" ? _userinfo.size() + _host.size() + 1 : _host.size();

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
  return _protocol == "" ? _protocol : _protocol + ":";
}

std::string URLString::GetAuthority() const
{
  if (_host == "")
    return "";

  std::string userinfo = _userinfo != "" ? _userinfo + "@" : "";
  std::string port = _port != "" ? ":" + _port : "";

  return "//" + userinfo + _host + port;
}

std::string URLString::GetPath() const
{
  return _path;
}

std::string URLString::GetQuery() const
{
  return _query == "" ? "" : "?" + _query;
}

std::string URLString::GetFragment() const
{
  return _fragment == "" ? "" : "#" + _fragment;
}

std::string URLString::operator()() const
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