#ifndef INCLUDED_HACKRF_COMMON_H
#define INCLUDED_HACKRF_COMMON_H

#include <vector>
#include <string>

#include <boost/format.hpp>
#include <boost/thread/mutex.hpp>

#include <libhackrf/hackrf.h>

#define BUF_LEN  (16 * 32 * 512) /* must be multiple of 512 */
#define BUF_NUM   15

#define BYTES_PER_SAMPLE  2 /* HackRF device consumes 8 bit unsigned IQ data */

#define HACKRF_FORMAT_ERROR(ret) \
  boost::str( boost::format("(%d) %s") \
    % ret % hackrf_error_name((enum hackrf_error)ret) ) \

#define HACKRF_THROW_ON_ERROR(ret, msg) \
  if ( ret != HACKRF_SUCCESS )  \
  throw std::runtime_error( boost::str( boost::format(msg " (%d) %s") \
      % ret % hackrf_error_name((enum hackrf_error)ret) ) );

#define HACKRF_FUNC_STR(func, arg) \
  boost::str(boost::format(func "(%d)") % arg) + " has failed"

class hackrf_common
{
public:
  hackrf_common();
  ~hackrf_common();

protected:
  static std::vector< std::string > devices();

  static hackrf_device *_dev;
  static std::vector<std::string> _devices;
  static int _usage;
  static boost::mutex _usage_mutex;
};

#endif
