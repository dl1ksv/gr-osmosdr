#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include "hackrf_common.h"

#include "arg_helpers.h"

hackrf_device *hackrf_common::_dev = NULL;
std::vector<std::string> hackrf_common::_devices;
int hackrf_common::_usage = 0;
boost::mutex hackrf_common::_usage_mutex;

hackrf_common::hackrf_common(const std::string &args)
{
  int ret;
  std::string hackrf_serial;

  dict_t dict = params_to_dict(args);

  {
    boost::mutex::scoped_lock lock( _usage_mutex );

    if ( _usage == 0 ) {
      hackrf_init(); /* call only once before the first open */

#ifdef LIBHACKRF_HAVE_DEVICE_LIST
      if (dict.count("hackrf") && dict["hackrf"].length() > 0) {
        hackrf_serial = dict["hackrf"];

        if (hackrf_serial.length() > 1) {
          ret = hackrf_open_by_serial( hackrf_serial.c_str(), &_dev );
        } else {
            int dev_index = 0;
            try {
              dev_index = boost::lexical_cast< int >( hackrf_serial );
            } catch ( std::exception &ex ) {
              throw std::runtime_error(
                    "Failed to use '" + hackrf_serial + "' as HackRF device index number: " + ex.what());
            }

            hackrf_device_list_t *list = hackrf_device_list();
            if (dev_index < list->devicecount) {
              ret = hackrf_device_list_open(list, dev_index, &_dev);
            } else {
              hackrf_device_list_free(list);
              throw std::runtime_error(
                    "Failed to use '" + hackrf_serial + "' as HackRF device index: not enough devices");
            }
            hackrf_device_list_free(list);
        }
      } else
#endif
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
        if ( ret != HACKRF_SUCCESS )
        {
          std::cerr << HACKRF_FORMAT_ERROR(ret, "Failed to close HackRF") << std::endl;
        }
        _dev = NULL;

        hackrf_exit(); /* call only once after last close */
      }
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
