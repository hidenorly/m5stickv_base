# m5stickv_hello_world

Based on https://github.com/sipeed/MaixPy/tree/master/projects/hello_world and https://github.com/sipeed/MaixPy/tree/master/projects/maixpy_m5stickv

Hello World for M5StickV.

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
$ ln -s /Users/xxx/work/hello_world_m5stickv /Users/xxx/work/MaixPy/projects
```

# How to build

```
$ cd MaixPy/projects/hello_world_m5stickv
$ python3 project.py clean_conf
-- SDK_PATH:/Users/xxxx/work/m5v/MaixPy
maixpy
clean now
$ python3 makemenuconfig
$ python3 project.py build
$ python3 project.py -B goE -p /dev/cu.usbserial-xxxx -b 1500000 flash
```
