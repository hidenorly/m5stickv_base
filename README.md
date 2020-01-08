# m5stickv_base

This is C language based embedded application framework for M5StickV.

Based on the following:
* https://github.com/sipeed/MaixPy/tree/master/projects/hello_world
* https://github.com/sipeed/MaixPy/tree/master/projects/maixpy_m5stickv
* https://github.com/hidenorly/hello_world_m5stickv


Current supported features:
* gpio
* LED (with Software PWM)
* Debug UART
* LCD
* Sensor support (MPU6886 and SH200Q)
  (My M5StickV has MPU6886 as IMU then please help to confirm about this functionality.)

# Get dependent modules

## Get MaixPy

Refer https://github.com/sipeed/MaixPy/blob/master/build.md

```
$ git clone https://github.com/sipeed/MaixPy.git
$ cd MaixPy
$ git submodule update --recursive --init
```

## Get and install tool chain

```
$ wget https://github.com/kendryte/kendryte-gnu-toolchain/releases/download/v8.2.0-20190409/kendryte-toolchain-osx-mojave-8.2.0-20190409.tar.bz2
$ sudo tar -Jxvf kendryte-toolchain-osx-mojave-8.2.0-20190409.tar.bz2 -C /opt
```

## Ensure pip requirements

```
$ pip3 install -r requirements.txt
```

## Copy to project folder

```
$ cp /Users/xxx/work/m5stickv_base /Users/xxx/work/MaixPy/projects
```

# How to build

```
$ cd MaixPy/projects/m5stickv_base
$ python3 project.py clean_conf
-- SDK_PATH:/Users/xxxx/work/MaixPy
maixpy
clean now
$ python3 project.py menuconfig
(Just Save & Quit)
$ python3 project.py build
$ python3 project.py -B goE -p /dev/cu.usbserial-xxxx -b 1500000 flash
```
