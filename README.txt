How to build

1. Get android open source.
    : version info - Android gingerbread 2.3.6
    ( Download site : http://source.android.com )

2. Overwrite modules that you want to build.

3. external/bluetooth  : Delete this source tree at Android gingerbread and then copy.
   external/iproute2   : Delete this source tree at Android gingerbread and then copy.

4. Add the following lines at the end of build/target/board/generic/BoardConfig.mk	
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_BCM := true

5. Write the following lines into build/core/user_tags.mk so that add these modules.
gatttool \
qcm_dut \	
ip \

6. make
- ./build.sh user

