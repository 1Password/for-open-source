#include <string>
#include <vector>

namespace license {
namespace test {

std::string generate_license(const std::string& fname, const std::vector<std::string>& other_args);
std::string sign_data(const std::string& data, const std::string& test_name);

}  // namespace test
}  // namespace license
