#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <set>
#include "httprequest.hpp"

auto valid_commands = {"GET", "POST", "HEAD"};

static std::string to_lower(const std::string& input) {
    std::string out;
    for (char c : input) out += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return out;
}

HTTPRequest::HTTPRequest(const std::string &request) {
    if (request.empty()) {
        throw MalformedRequestException("Empty request");
    }

    size_t end_of_header = request.find("\r\n\r\n");
    if (end_of_header == std::string::npos) {
        throw MalformedRequestException("Missing \\r\\n\\r\\n");
    }

    std::vector<std::string> lines;
    size_t start = 0;
    for (size_t i = 1; i < end_of_header + 2; ++i) {
        if (request[i-1] == '\r' && request[i] == '\n') {
            lines.push_back(request.substr(start, i - 1 - start));
            start = i + 1;
        }
    }

    if (lines.empty()) {
        throw MalformedRequestException("Missing command line");
    }

    parse_command(lines[0]);

    std::set<std::string> seen_headers;

    for (size_t i = 1; i < lines.size(); ++i) {
        std::shared_ptr<HTTPHeader> header = std::make_shared<HTTPHeader>(lines[i]);
        std::string name = to_lower(header->get_name());

        if (seen_headers.count(name)) {
            throw MalformedRequestException("Duplicate header: " + name);
        }
        seen_headers.insert(name);
        headers[name] = header;
    }

    payload = request.substr(end_of_header + 4);
}

void HTTPRequest::parse_command(const std::string &command_string) {
  size_t first_space = command_string.find(' ');
  size_t second_space = command_string.find(' ', first_space + 1);
  
  // Must have exactly two spaces
  if (first_space == std::string::npos || second_space == std::string::npos ||
      command_string.find(' ', second_space + 1) != std::string::npos) {
      throw MalformedRequestException("Invalid number of spaces in command line");
  }
  
  std::string method = command_string.substr(0, first_space);
  std::string res = command_string.substr(first_space + 1, second_space - first_space - 1);
  std::string version = command_string.substr(second_space + 1);
  

    if (std::find(valid_commands.begin(), valid_commands.end(), method) == valid_commands.end()) {
        throw MalformedRequestException("Invalid method: " + method);
    }

    if (version != "HTTP/1.1") {
        throw MalformedRequestException("Invalid version");
    }

    command = method;
    resource = res;
}

HTTPHeader::HTTPHeader(const std::string &s) {
    size_t colon_pos = s.find(':');
    if (colon_pos == std::string::npos) {
        throw MalformedRequestException("Invalid header format");
    }

    name = s.substr(0, colon_pos);
    name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

    std::string raw_value = s.substr(colon_pos + 1);


    size_t start = 0;
    while (start < raw_value.size() && std::isspace(static_cast<unsigned char>(raw_value[start]))) ++start;


    size_t end = raw_value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(raw_value[end - 1]))) --end;

    value = raw_value.substr(start, end - start);

}
