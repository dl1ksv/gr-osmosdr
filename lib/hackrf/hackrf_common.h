#ifndef INCLUDED_HACKRF_COMMON_H
#define INCLUDED_HACKRF_COMMON_H

#include <vector>
#include <string>

#include <boost/thread/mutex.hpp>

#include <libhackrf/hackrf.h>

class hackrf_common
{
public:
  hackrf_common();
  ~hackrf_common();

protected:
  static std::vector< std::string > devices();

  static int _usage;
  static boost::mutex _usage_mutex;
};

#endif
