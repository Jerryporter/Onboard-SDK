/*! @file camera_stream_poll_sample.cpp
 *  @version 4.0.0
 *  @date Feb 1st 2018
 *
 *  @brief
 *  AdvancedSensing, Camera Stream API usage in a Linux environment.
 *  This sample shows how to poll new images from FPV camera or/and
 *  main camera from the main thread.
 *  (Optional) With OpenCV installed, user can visualize the images
 *
 *  @Copyright (c) 2017 DJI
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * */
#define BOOST_ASIO_DISABLE_STD_CHRONO
#define BOOST_CHRONO_HEADER_ONLY
#define BOOST_CHRONO_EXTENSIONS
#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#define BOOST_THREAD_VERSION 4
// #define RAPIDJSON_HAS_STDSTRING
#include <boost/asio.hpp>
#include <boost/asio/high_resolution_timer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/system_timer.hpp>
#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>
#include <boost/timer/timer.hpp>
#include <boost/utility/string_ref.hpp>

#include "base64.h"
#include "json.hpp"

#include <iostream>
#include <string>
#include <vector>
using namespace boost::system;
using namespace boost::asio;
using namespace boost;
using json = nlohmann::json;

using boost::shared_ptr;
using boost::asio::ip::udp;
using std::cin;
using std::cout;
using std::endl;
using std::vector;
// using namespace rapidjson;

#include "dji_linux_helpers.hpp"
#include "dji_vehicle.hpp"
#include <iostream>

// #ifdef OPEN_CV_INSTALLED
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
using namespace cv;
// #endif

using namespace DJI::OSDK;

#define DELAY_SECOND 1

void
show_rgb(CameraRGBImage                     img,
         char*                              name,
         boost::shared_ptr<ip::tcp::socket> sock_ptr)
{
  // #ifdef OPEN_CV_INSTALLED
  // imgFile.open("/home/dji/Downloads/Onboard-SDK-master/sample/platform/linux/"
  //              "advanced-sensing/camera_stream_poll_sample/imgFile.JPG",
  //              ios::out | ios::binary);
  // cout<<rawdata_str;

  // ofstream imgFile;
  // imgFile << rawdata_str;
  // imgFile.flush();

  cout << "#### Got image from:\t" << string(name) << endl;
  Mat mat(img.height, img.width, CV_8UC3, img.rawData.data(), img.width * 3);

  cout << "size is " << img.rawData.size() << endl;
  cout << "height" << img.height << "width" << img.width << endl;

  cvtColor(mat, mat, COLOR_RGB2BGR);

  vector<uint8_t> rawdata_convert;
  vector<int>     compression_params;
  compression_params.push_back(IMWRITE_PNG_COMPRESSION);
  compression_params.push_back(9);
  try
  {
    imencode(".png", mat, rawdata_convert, compression_params);

    string imgEncodeString;
    imgEncodeString.assign(rawdata_convert.begin(), rawdata_convert.end());
    std::string img_mess = base64_encode(
      reinterpret_cast<const unsigned char*>(imgEncodeString.c_str()),
      imgEncodeString.length());

    nlohmann::json imgPack_j;
    imgPack_j["protocol"]    = "image_file_drone";
    imgPack_j["data"]["vga"] = img_mess;

    string imgPack_str = imgPack_j.dump();
    int    length      = imgPack_str.length();

    cout << sock_ptr->send(boost::asio::buffer((void*)&length, sizeof(length)))
         << endl;
    cout << sock_ptr->send(boost::asio::buffer(imgPack_str)) << endl;
  }
  catch (cv::Exception& ex)
  {
    fprintf(
      stderr, "Exception converting image to PNG format: %s\n", ex.what());
    return;
  }
  // imshow(name, mat);
  cv::waitKey(1);
  // #endif
}

int
main(int argc, char** argv)
{
  io_service        io;
  ip::tcp::endpoint ep(ip::address::from_string("192.168.2.74"), 8899);
  // ip::tcp::socket   sock(io);
  boost::shared_ptr<ip::tcp::socket> sock(new ip::tcp::socket(io));
  sock->connect(ep);
  bool f = false;
  bool m = false;
  char c = 0;
  cout << "hello ??" << endl;
  cout << "Please enter the type of camera stream you want to view(M210 V2)\n"
       << "m: Main Camera\n"
       << "f: FPV  Camera\n"
       << "b: both Cameras" << endl;
  cin >> c;
  // c='f';
  switch (c)
  {
    case 'm':
      m = true;
      break;
    case 'f':
      f = true;
      break;
    case 'b':
      f = true;
      m = true;
      break;
    default:
      cout << "No camera selected";
      return 1;
  }

  // Setup OSDK.
  bool        enableAdvancedSensing = true;
  LinuxSetup  linuxEnvironment(argc, argv, enableAdvancedSensing);
  Vehicle*    vehicle = linuxEnvironment.getVehicle();
  const char* acm_dev =
    linuxEnvironment.getEnvironment()->getDeviceAcm().c_str();
  vehicle->advancedSensing->setAcmDevicePath(acm_dev);
  if (vehicle == NULL)
  {
    std::cout << "Vehicle not initialized, exiting.\n";
    return -1;
  }

  bool fpvCamResult  = false;
  bool mainCamResult = false;

  if (f)
  {
    fpvCamResult = vehicle->advancedSensing->startFPVCameraStream();
    if (!fpvCamResult)
    {
      cout << "Failed to open FPV Camera" << endl;
    }
  }

  if (m)
  {
    mainCamResult = vehicle->advancedSensing->startMainCameraStream();
    if (!mainCamResult)
    {
      cout << "Failed to open Main Camera" << endl;
    }
  }

  if ((!fpvCamResult) && (!mainCamResult))
  {
    cout << "Failed to Open Either Cameras, exiting" << endl;
    return 1;
  }

  CameraRGBImage fpvImg;
  CameraRGBImage mainImg;
  char           fpvName[]  = "FPV_CAM";
  char           mainName[] = "MAIN_CAM";

  steady_timer t(io);
  for (int i = 0; i < 1000; i++)
  {
    if (i == 0)
    {
      t.expires_after(boost::chrono::seconds(2));
      t.wait();
    }
    if (f && fpvCamResult &&
        vehicle->advancedSensing->newFPVCameraImageIsReady())
    {
      if (vehicle->advancedSensing->getFPVCameraImage(fpvImg))
      {
        t.expires_after(boost::chrono::seconds(DELAY_SECOND));
        t.wait();
        show_rgb(fpvImg, fpvName, sock);
      }
      else
      {
        cout << "Time out" << endl;
      }
    }

    if (m && mainCamResult &&
        vehicle->advancedSensing->newMainCameraImageReady())
    {
      if (vehicle->advancedSensing->getMainCameraImage(mainImg))
      {
        t.expires_after(boost::chrono::seconds(DELAY_SECOND));
        t.wait();
        show_rgb(mainImg, mainName, sock);
      }
      else
      {
        cout << "Time out" << endl;
      }
    }
    usleep(2e4);
  }

  if (f)
  {
    vehicle->advancedSensing->stopFPVCameraStream();
  }
  else if (m)
  {
    vehicle->advancedSensing->stopMainCameraStream();
  }

  return 0;
}
