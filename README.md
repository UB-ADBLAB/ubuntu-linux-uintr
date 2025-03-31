# What is this:

Patched Linux kernel (from Ubuntu-hwe-6.11-6.11.0-24.24_24.04-1) with builtin
uintr support for x86 user interrupts, based on the archived Intel uintr kernel
implementation at https://github.com/intel/uintr-linux-kernel (from
4fe89d07dcc2804c8b562f6c7896a45643d34b2f to
0ee776bd38532358159013ed0188693b34c46cf5).

# How to build

    cp myconfig .config
    make -j NPROCS # replace NPROCS with the number of parallel processes you'd like
    make bzImage
    sudo make modules_install
    sudo make install
    sudo make headers_install ARCH=x86 INSTALL_HDR_PATH=/usr/uintr-kernel # installs to /usr/uintr-kernel/include
