// ParserVariant.cpp 

#include <iostream>
#include <filesystem>
#include <variant>
#include <unordered_map>
#include <optional>
#include <fstream>

using VariantMap = std::unordered_map<std::string, std::variant<int, bool, std::string>>;
namespace fs = std::filesystem;

class Parser {
private:
	fs::path m_path;
	bool fileExist;

public:

	Parser() : m_path("D:\\C++_projects\\ParserVariant\\data.txt"), fileExist(true) {};
	Parser(fs::path a_path) : m_path(a_path) { 
		fs::exists(m_path) ? 
			fileExist = true : fileExist = false;
	};

	std::optional<VariantMap> parse() const
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

		VariantMap result;
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

			std::cout << "Key: " << key << std::endl;
			std::cout << "Value: " << value << std::endl;

			std::variant<int, bool, std::string> variant;

			int ivalue;
			auto [ptr, erc] = std::from_chars(value.data(), value.data() + value.size(), ivalue);
			if (erc == std::errc())
				variant = ivalue;
			else if (value == "true")
				variant = true;
			else if (value == "false")
				variant = false;
			else
				variant = value.data();

			result.insert(std::make_pair(key, variant));

			start = enter + 1;
		}

		return result;
	}

};

int main()
{
	Parser p;
	auto res = p.parse();
	/*if (res.has_value()) {
		for (auto [key, value] : res.value()) {
			//std::cout << "Key: " << key << " =  Value: ";
			if (int* val = std::get_if<int>(&value)) {
				std::cout << "Value: " << *val << " END";
			}
			else if (bool* val = std::get_if<bool>(&value)) {
				std::cout << "Value: " << *val << " END";
			}
			else if (std::string* val = std::get_if<std::string>(&value)) {
				std::cout << "Value: " << *val << " END";
			}
			std::cout << std::endl;
		}
	}*/
	system("PAUSE");
	return 0;
}

