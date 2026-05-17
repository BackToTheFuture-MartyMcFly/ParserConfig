// ParserVariant.cpp 

#include <iostream>
#include <filesystem>
#include <variant>
#include <unordered_map>
#include <optional>
#include <fstream>

namespace fs = std::filesystem;

std::vector<std::string_view> split(std::string_view str, char delimiter) {
	std::vector<std::string_view> result;
	size_t start = 0;
	size_t end = str.find(delimiter);

	while (end != std::string_view::npos) {
		if (end != start) {  
			result.push_back(str.substr(start, end - start));
		}
		start = end + 1;
		end = str.find(delimiter, start);
	}

	if (start < str.size()) { 
		result.push_back(str.substr(start));
	}

	return result;
}

class ConfigNode;
using ConfigValue = std::variant<int, bool, std::string, ConfigNode>;

class ConfigNode {

private:
	std::unordered_map<std::string, ConfigValue> m_values;

public:

	void insertValue(const std::string_view& key, const ConfigValue& value) {
		auto parts = split(key, '.');
		ConfigNode* current = this;

		for (size_t i = 0; i < parts.size(); ++i) {
			std::string part(parts[i]);
			if (i == parts.size() - 1) {
				current->m_values[part] = value;
			}
			else {
				if (!current->m_values.contains(part)) {
					current->m_values[part] = ConfigNode{};
				}
				current = &std::get<ConfigNode>(current->m_values[part]);
			}
		}
	}

	void print(int indent = 0) const {
		const std::string indentStr(indent * 2, ' ');

		for (const auto& [key, value] : m_values) {
			if (const ConfigNode* child = std::get_if<ConfigNode>(&value)) {
				std::cout << indentStr << key << ":\n";
				child->print(indent + 1);
			}
			else {
				std::cout << indentStr << key << " = ";

				if (const int* val = std::get_if<int>(&value)) {
					std::cout << *val;
				}
				else if (const bool* val = std::get_if<bool>(&value)) {
					std::cout << std::boolalpha << *val;
				}
				else if (const std::string* val = std::get_if<std::string>(&value)) {
					std::cout << *val;
				}
				std::cout << std::endl;
			}
		}
	}

	template<typename T>
	std::optional<T> getValue(std::string_view key) const {
		auto parts = split(key, '.');
		const ConfigNode* current = this;

		for (size_t i = 0; i < parts.size(); ++i) {
			std::string part(parts[i]);
			if (!current->m_values.contains(part))	// C++20
				return std::nullopt;


			auto it = current->m_values.find(part);
			if (i == parts.size() - 1) {
					if (std::holds_alternative<T>(it->second))
						return std::get<T>(it->second);
					return std::nullopt;
			}
			else {
				if (!std::holds_alternative<ConfigNode>(it->second)) return std::nullopt;
				current = &std::get<ConfigNode>(it->second);
			}
		}
	
	}

};


class Parser {
private:
	fs::path m_path;
	bool fileExist;

public:

	Parser() : m_path("..\\data.txt"), fileExist(true) {};
	Parser(fs::path a_path) : m_path(a_path) { 
		fs::exists(m_path) ? 
			fileExist = true : fileExist = false;
	};

	std::optional<ConfigNode> parse() const
	{
		if (!fileExist)
			return std::nullopt;

		std::ifstream file(m_path);
		if (!file.is_open()) {
			return std::nullopt;
		}

		auto size = fs::file_size(m_path);
		std::string content;
		content.resize(size);
		file.read(content.data(), size);

		ConfigNode result;
		std::string_view strV(content);
		size_t start = 0;

		while (start < strV.size()) {
			size_t eq = strV.find('=',start);
			if (eq == std::string_view::npos)
				break;
			size_t enter = strV.find('\n', eq);
			if (enter == std::string_view::npos) {
				enter = strV.size();
			}

			std::string_view key = strV.substr(start, eq - start - 1);
			std::string_view value = strV.substr(eq + 2, enter - eq - 2);

			ConfigValue variant;

			int ivalue;
			auto [ptr, erc] = std::from_chars(value.data(), value.data() + value.size(), ivalue);
			if (erc == std::errc() && ptr == value.data() + value.size())
				variant = ivalue;
			else if (value == "true")
				variant = true;
			else if (value == "false")
				variant = false;
			else
				variant = std::string(value);

			result.insertValue(key, variant);

			start = enter + 1;						
		}

		return result;
	}

};

int main()
{
	/*ConfigNode node;
	node.insertValue("server.ip", "10.98.67.2");
	node.insertValue("server.port", "8080");
	node.insertValue("server.reconnect", true);
	node.insertValue("metadata.resolve", "yes");
	node.insertValue("q.w.e.r.t.y.u.io", 4);

	ConfigNode node1 = node.getValue<ConfigNode>(std::string("server")).value_or(ConfigNode{});
	node1.print();*/
	Parser parser;
	ConfigNode node = parser.parse().value_or(ConfigNode{});

	node.print();
	
	system("PAUSE");
	return 0;
}

