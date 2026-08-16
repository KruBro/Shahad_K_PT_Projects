# ELARM Wi-Fi Driver Bring-Up (QEMU / vexpress-a9)

Embedded Linux Wi-Fi interface bring-up project, built from scratch: U-Boot,
mainline Linux kernel, and a Buildroot root filesystem, running on a QEMU-emulated
ARM Cortex-A9 (Versatile Express) board. Wireless is provided by `mac80211_hwsim`,
the kernel's software-simulated radio driver, allowing a full WPA2 access-point
and client handshake to be demonstrated without any physical hardware.

This deviates from the original project brief (Beaglebone Black + real Wi-Fi
dongle) by targeting QEMU + simulated radios instead — chosen deliberately to
focus on understanding the embedded Linux boot chain and wireless stack without
hardware-specific risk (bricking, USB passthrough issues, etc).

## Result

Two independent simulated wireless radios (`wlan0` acting as an access point via
`hostapd`, `wlan1` acting as a client via `wpa_supplicant`) complete a real
WPA2-PSK association, and exchange live TCP/IP traffic (verified via `ping`)
over the simulated wireless link — fulfilling the original project goal of
Wi-Fi interface bring-up with successful data exchange.

```
wlan1: authenticated
wlan1: associated
iw dev wlan1 link
  Connected to 02:00:00:00:00:00 (on wlan1)
  SSID: TestNetwork
  freq: 2412.0
  signal: -30 dBm

ping -c 4 192.168.50.1
  4 packets transmitted, 4 packets received, 0% packet loss
```

## Architecture

```
QEMU (vexpress-a9, Cortex-A9, 256MB RAM)
  |
  v
U-Boot 2025.07          <- first-stage bootloader, loads kernel+dtb, sets bootargs
  |
  v
Linux kernel 6.12.27 LTS <- cross-compiled with Buildroot's toolchain
  |  (cfg80211 + mac80211 + mac80211_hwsim modules)
  v
Buildroot rootfs (ext2)  <- busybox, iw, wpa_supplicant, hostapd
  |
  v
hostapd (wlan0, AP) <----WPA2----> wpa_supplicant (wlan1, client)
```

| Component | Version | Purpose |
|---|---|---|
| Buildroot | 2025.02.x | Builds cross-toolchain, kernel, root filesystem |
| Linux kernel | 6.12.27 (LTS) | OS kernel, wireless subsystem |
| U-Boot | 2025.07 | Bootloader |
| QEMU | 10.2.2 | ARM board emulation (`vexpress-a9` machine) |
| mac80211_hwsim | in-kernel | Simulated Wi-Fi radio driver |

## Repository layout

```
configs/
  buildroot.config        - Buildroot .config (board, kernel version, toolchain, packages)
  wireless/
    hostapd.conf           - AP configuration used on wlan0
    wpa_supplicant.conf    - Client configuration used on wlan1
boot-vexpress.sh            - Boots the system in QEMU (device-loader method)
README.md                   - This file
NOTES.md                    - Full command log, debugging notes, and lessons learned
```

Full source trees (`buildroot/`, `u-boot/`, kernel source) are intentionally
**not** committed — they are large, third-party, and reproducible from the
configs above. See NOTES.md for exact build commands to regenerate them.

## Quick start (reproducing this build)

```bash
# 1. Buildroot: toolchain + kernel + rootfs
git clone https://github.com/buildroot/buildroot.git && cd buildroot
git checkout 2025.02.x
cp ../configs/buildroot.config .config
make menuconfig   # optional: verify config, exit+save
make              # ~30-60 min first run

# 2. U-Boot
cd ..
git clone https://github.com/u-boot/u-boot.git && cd u-boot
git checkout v2025.07
make vexpress_ca9x4_defconfig
make -j$(nproc) ARCH=arm CROSS_COMPILE=$(pwd)/../buildroot/output/host/bin/arm-linux-

# 3. Boot
cd ..
mkdir -p boot-files
cp buildroot/output/images/{zImage,vexpress-v2p-ca9.dtb,rootfs.ext2} boot-files/
./boot-vexpress.sh
```

At the U-Boot `=>` prompt:
```
printenv fdtcontroladdr
setenv bootargs console=ttyAMA0,38400n8 root=/dev/mmcblk0 rootfstype=ext2 rw
bootz 0x60100000 - $fdtcontroladdr
```

Log in as `root` (no password), then bring up wireless:
```
modprobe cfg80211
modprobe mac80211
modprobe mac80211_hwsim radios=2
ip link set wlan0 up
ip link set wlan1 up
hostapd -B /path/to/hostapd.conf
wpa_supplicant -B -i wlan1 -c /path/to/wpa_supplicant.conf
ip addr add 192.168.50.1/24 dev wlan0
ip addr add 192.168.50.2/24 dev wlan1
ping -c 4 192.168.50.1
```

See `NOTES.md` for the complete annotated command reference, including the
debugging process for two real issues hit during bring-up (U-Boot binary format
and device-tree loading address), and why kernel wireless config wasn't visible
in `menuconfig` until dependencies were satisfied.

## References

- https://wiki.debian.org/WiFi
- https://www.kernel.org/doc/html/latest/networking/mac80211_hwsim/
