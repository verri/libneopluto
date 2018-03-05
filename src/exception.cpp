#include "neopluto/exception.hpp"

namespace npl
{

sqlite_exception::sqlite_exception(const char* routine, int retval, const char* msg)
  : routine{routine}, retval{retval}, msg{msg}
{
}

auto sqlite_exception::what() const noexcept -> const char* { return "SQLite exception"; }

} // namespace npl
