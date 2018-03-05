#ifndef NEOPLUTO_EXCEPTION_HPP
#define NEOPLUTO_EXCEPTION_HPP

#include <exception>

namespace npl
{

struct sqlite_exception : public std::exception {
  sqlite_exception(const char* routine, int retval, const char* msg = nullptr);
  auto what() const noexcept -> const char*;

  const char* routine;
  int retval;
  const char* msg;
};

} // namespace npl

#endif // NEOPLUTO_EXCEPTION_HPP
