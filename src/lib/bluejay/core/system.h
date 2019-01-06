#include <boost/filesystem/path.hpp>

namespace bluejay {
namespace system {

  boost::filesystem::path program_location();

  boost::filesystem::path libpath() {
    return program_location().parent_path().parent_path() / "lib";
  }

}
}
