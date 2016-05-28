#ifndef INCLUDED_HACKRF_COMMON_H
#define INCLUDED_HACKRF_COMMON_H

#include <vector>
#include <string>

#include <boost/thread/mutex.hpp>

#include <libhackrf/hackrf.h>

#define BUF_LEN  (16 * 32 * 512) /* must be multiple of 512 */
#define BUF_NUM   15

#define BYTES_PER_SAMPLE  2 /* HackRF device produces/consumes 8 bit unsigned IQ data */

#define HACKRF_FORMAT_ERROR(ret, msg) \
  boost::str( boost::format(msg " (%1%) %2%") \
    % ret % hackrf_error_name((enum hackrf_error)ret) )

#define HACKRF_THROW_ON_ERROR(ret, msg) \
  if ( ret != HACKRF_SUCCESS ) \
  { \
    throw std::runtime_error( HACKRF_FORMAT_ERROR(ret, msg) ); \
  }

#define HACKRF_FUNC_STR(func, arg) \
  boost::str(boost::format(func "(%1%)") % arg) + " has failed"

class hackrf_common
{
public:
  hackrf_common(const std::string &args);
  ~hackrf_common();

protected:
  static std::vector< std::string > get_devices();

  static hackrf_device *_dev;
  static std::vector<std::string> _devices;
  static int _usage;
  static boost::mutex _usage_mutex;

  static bool set_bias( bool enabled, size_t chan = 0 );
};

#endif /* INCLUDED_HACKRF_COMMON_H */
