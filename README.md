
[![Demo](https://img.youtube.com/vi/DZRnGjES52k/0.jpg)](https://youtu.be/DZRnGjES52k)

## Building and running (on Debian)

``` bash
sudo apt-get install libboost-all-dev libsdl2-dev libsdl2-ttf-dev cmake
git clone https://github.com/acriaer/tetris3d.git
cd tetris3d
mkdir build
cd build
cmake ..
make -j4
cd ..
./build/tetris [--<option>=value]*
```

## Options
 - resx
 - resy
 - fullscreen
 - initial_speed 
 - max_speed 
 - boost_speed 
 - height 
 - speed_increment
 - speed_increment_peroid

 ## Controls

  - W ; S -- rotate falling block vertically
  - A ; D -- rotate falling block horizontally
  - I ; K -- move falling block forward/backward
  - J ; L -- move falling block left/right
  - Q ; E -- rotate camera
  - , ; . -- zoom in / zoom out
  - SPACE -- boost falling
  - ESC -- quit