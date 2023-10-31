#ifndef DEFINE_H
#define DEFINE_H

//////////////////////////////////////////////////
// Select a communication middleware
//
//#define USE_TRANSPORT_ECAL
#define USE_TRANSPORT_GRPC

//////////////////////////////////////////////////
// Select a robot type
//
//#define ROBOT_QUADIP
#define ROBOT_WOLY

#ifdef ROBOT_QUADIP
#define ROBOT_NAME "Quad-IP"
#define APP_NAME "ArtPlot (Quad-IP)"
#endif
#ifdef ROBOT_WOLY
#define ROBOT_NAME "Woly"
#define APP_NAME "ArtPlot (Woly)"
#endif

#endif // DEFINE_H