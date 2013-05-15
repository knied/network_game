Compile:

1.) Install Qt 5.0.2
2.) $mkdir build
3.) $cd build
4.) $~/Qt5.0.2/5.0.2/clang_64/bin/qmake ../NetworkGameClient.pro
5.) $make

Deploying (For execution on mashines other than the development mashine)

1.) $mkdir build-release
2.) $cd build-release
3.) $~/Qt5.0.2/5.0.2/clang_64/bin/qmake -config release ../NetworkGameClient.pro
4.) $~/Qt5.0.2/5.0.2/clang_64/bin/macdeployqt NetworkGameClient.app

