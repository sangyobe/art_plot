#ifndef DEFINE_H
#define DEFINE_H

//////////////////////////////////////////////////
// Select a communication middleware
// (This may be provided as CMake option)
//#define USE_TRANSPORT_ECAL
//#define USE_TRANSPORT_GRPC

//////////////////////////////////////////////////
// Select a robot type
// (This may be provided as CMake option)
//#define ROBOT_QUADIP
//#define ROBOT_WOLY
//#define ROBOT_LEOQUAD

#ifdef ROBOT_QUADIP
#define ROBOT_NAME "Quad-IP"
#define APP_NAME "ArtPlot (Quad-IP)"
#endif
#ifdef ROBOT_WOLY
#define ROBOT_NAME "Woly"
#define APP_NAME "ArtPlot (Woly)"
#endif
#ifdef ROBOT_LEOQUAD
#define ROBOT_NAME "LEOquad"
#define APP_NAME "ArtPlot (LEOquad)"
#endif

#endif // DEFINE_H