#include <boost/dll/runtime_symbol_info.hpp>

namespace bluejay {
namespace system {

  boost::filesystem::path program_location() {
    return boost::dll::program_location();
  }

}
}
