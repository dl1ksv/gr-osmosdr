#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include "hackrf_common.h"

hackrf_device *hackrf_common::_dev = NULL;
std::vector<std::string> hackrf_common::_devices;
int hackrf_common::_usage = 0;
boost::mutex hackrf_common::_usage_mutex;

hackrf_common::hackrf_common()
{

}

hackrf_common::~hackrf_common()
{
  if (_dev) {
//    _thread.join();
    int ret = hackrf_close( _dev );
    HACKRF_THROW_ON_ERROR(ret, "Failed to close HackRF")
    _dev = NULL;

    {
      boost::mutex::scoped_lock lock( _usage_mutex );

       _usage--;

      if ( _usage == 0 )
        hackrf_exit(); /* call only once after last close */
    }
  }
}

std::vector<std::string> hackrf_common::get_devices()
{
  if (!_devices.empty()) return _devices;

  std::string label;

  {
    boost::mutex::scoped_lock lock( _usage_mutex );

    if ( _usage == 0 )
      hackrf_init(); /* call only once before the first open */

    _usage++;
  }

#ifdef LIBHACKRF_HAVE_DEVICE_LIST
  hackrf_device_list_t *list = hackrf_device_list();

  for (int i = 0; i < list->devicecount; i++) {
    label = "HackRF ";
    label += hackrf_usb_board_id_name( list->usb_board_ids[i] );

    std::string args;
    if (list->serial_numbers[i]) {
      std::string serial = boost::lexical_cast< std::string >( list->serial_numbers[i] );
      if (serial.length() > 6)
        serial = serial.substr(serial.length() - 6, 6);
      args = "hackrf=" + serial;
      label += " " + serial;
    } else
      args = "hackrf"; /* will pick the first one, serial number is required for choosing a specific one */

    boost::algorithm::trim(label);

    args += ",label='" + label + "'";
    _devices.push_back( args );
  }

  hackrf_device_list_free(list);
#else

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

#endif

  {
    boost::mutex::scoped_lock lock( _usage_mutex );

     _usage--;

    if ( _usage == 0 )
      hackrf_exit(); /* call only once after last close */
  }

  return _devices;
}
