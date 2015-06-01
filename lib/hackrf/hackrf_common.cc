#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "hackrf_common.h"

hackrf_device *hackrf_common::_dev = NULL;
std::vector<std::string> hackrf_common::_devices;
int hackrf_common::_usage = 0;
boost::mutex hackrf_common::_usage_mutex;

hackrf_common::hackrf_common()
{
  int ret;

  {
    boost::mutex::scoped_lock lock( _usage_mutex );

    if ( _usage == 0 ) {
      hackrf_init(); /* call only once before the first open */
      ret = hackrf_open( &_dev );
      HACKRF_THROW_ON_ERROR(ret, "Failed to open HackRF device")

      uint8_t board_id;
      ret = hackrf_board_id_read( _dev, &board_id );
      HACKRF_THROW_ON_ERROR(ret, "Failed to get HackRF board id")

      char version[40];
      memset(version, 0, sizeof(version));
      ret = hackrf_version_string_read( _dev, version, sizeof(version));
      HACKRF_THROW_ON_ERROR(ret, "Failed to read version string")
#if 0
      read_partid_serialno_t serial_number;
      ret = hackrf_board_partid_serialno_read( _dev, &serial_number );
      HACKRF_THROW_ON_ERROR(ret, "Failed to read serial number")
#endif
      std::cerr << "Using " << hackrf_board_id_name(hackrf_board_id(board_id)) << " "
                << "with firmware " << version << " "
                << std::endl;
    }

    _usage++;
  }
}

hackrf_common::~hackrf_common()
{
  if (_dev) {
//    _thread.join();

    {
      boost::mutex::scoped_lock lock( _usage_mutex );

       _usage--;

      if ( _usage == 0 ) {
        int ret = hackrf_close( _dev );
        HACKRF_THROW_ON_ERROR(ret, "Failed to close HackRF")
        _dev = NULL;

        hackrf_exit(); /* call only once after last close */
      }
    }
  }
}

std::vector<std::string> hackrf_common::devices()
{
  if (!_devices.empty()) return _devices;

  std::string label;
#if 0
  for (unsigned int i = 0; i < 1 /* TODO: missing libhackrf api */; i++) {
    std::string args = "hackrf=" + boost::lexical_cast< std::string >( i );

    label.clear();

    label = "HackRF Jawbreaker"; /* TODO: missing libhackrf api */

    boost::algorithm::trim(label);

    args += ",label='" + label + "'";
    _devices.push_back( args );
  }
#else

  {
    boost::mutex::scoped_lock lock( _usage_mutex );

    if ( _usage == 0 )
      hackrf_init(); /* call only once before the first open */

    _usage++;
  }

  int ret;
  hackrf_device *dev = NULL;
  ret = hackrf_open(&dev);
  if ( HACKRF_SUCCESS == ret )
  {
    std::string args = "hackrf=0";

    label = "HackRF";

    uint8_t board_id;
    ret = hackrf_board_id_read( dev, &board_id );
    if ( HACKRF_SUCCESS == ret )
    {
      label += std::string(" ") + hackrf_board_id_name(hackrf_board_id(board_id));
    }

    args += ",label='" + label + "'";
    _devices.push_back( args );

    ret = hackrf_close(dev);
  }

  {
    boost::mutex::scoped_lock lock( _usage_mutex );

     _usage--;

    if ( _usage == 0 )
      hackrf_exit(); /* call only once after last close */
  }

#endif
  return _devices;
}
