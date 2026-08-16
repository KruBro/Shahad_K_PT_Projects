# Build & Debug Notes

Full, annotated log of commands used, in the order they were actually run,
including the debugging detours that came up along the way.

## Host setup (Fedora 44)

```bash
sudo dnf install -y qemu-system-arm qemu-img git make gcc gcc-c++ ncurses-devel \
  bison flex bc python3 rsync wget unzip cpio which perl-ExtUtils-MakeMaker \
  openssl-devel dtc
sudo dnf install -y tftp-server tftp   # for the TFTP boot path (see below)
```

## Phase 1: Buildroot (toolchain + kernel + rootfs)

```bash
git clone https://github.com/buildroot/buildroot.git
cd buildroot
git checkout 2025.02.x
make qemu_arm_vexpress_defconfig
make menuconfig
```

Config decisions made in `menuconfig`:
- Kernel version: `6.12.27` (LTS) — chosen over the bleeding-edge 7.1.x series
  for maturity, community tutorials, and predictability on an older board target.
- Kernel configuration: in-tree `vexpress` defconfig (fragment shipped with kernel source)
- Device tree: `arm/vexpress-v2p-ca9`, DTB built automatically by Buildroot
- Filesystem: `ext2/3/4`, ext2 variant, 64MB fixed size

```bash
make    # first build: ~30-60 min, builds cross-toolchain + kernel + rootfs
```

Output artifacts in `output/images/`: `zImage`, `vexpress-v2p-ca9.dtb`, `rootfs.ext2`

## Phase 2: U-Boot

```bash
git clone https://github.com/u-boot/u-boot.git
cd u-boot
git checkout v2025.07
make vexpress_ca9x4_defconfig
export CROSS_COMPILE=<path-to-buildroot>/output/host/bin/arm-linux-
export ARCH=arm
make -j$(nproc)
```

**Important finding:** the raw `u-boot.bin` loaded via QEMU's `-bios` or `-kernel`
flag produced a silent 100%-CPU hang with zero console output on this QEMU
version (10.2.2). The **ELF binary** (`u-boot`, no extension) loaded via
`-kernel` booted correctly instead — the ELF's embedded entry point avoided
whatever address/format mismatch the raw binary hit. If reproducing this
build on a different QEMU version, verify which form actually boots before
assuming the raw `.bin` works.

## Phase 3: First boot (device-loader method)

```bash
qemu-system-arm -M vexpress-a9 -m 256M -nographic \
  -kernel u-boot/u-boot \
  -device loader,file=boot-files/zImage,addr=0x60100000 \
  -device loader,file=boot-files/vexpress-v2p-ca9.dtb,addr=0x60000000
```

**Debugging note:** the first `bootz 0x60100000 - 0x60000000` failed with
`Could not find a valid device tree`, even though the dtb file itself was
valid (`xxd` confirmed the `d00dfeed` magic number on disk). Diagnosis:

```
=> fdt addr 0x60000000
libfdt fdt_check_header(): FDT_ERR_BADMAGIC
```

The two `-device loader` directives didn't land where expected — QEMU's own
default dtb handling for this machine type interfered. Fix: use QEMU's
dedicated `-dtb` flag instead, then discover the *actual* load address U-Boot
sees via the `fdtcontroladdr` environment variable, rather than assuming
`0x60000000`:

```
=> fdt addr $fdtcontroladdr
Working FDT set to 6ffe12f0
```

Working boot sequence:
```
setenv bootargs console=ttyAMA0,38400n8 root=/dev/mmcblk0 rootfstype=ext2 rw
bootz 0x60100000 - $fdtcontroladdr
```

This produced a kernel panic (`Unable to mount root fs on unknown-block(0,0)`)
because no disk was attached yet — expected at this stage, and confirmed the
kernel/dtb/U-Boot chain itself was working correctly (reached `kernel_init`).

Final working QEMU invocation, with rootfs attached as a virtual SD card
(saved as `boot-vexpress.sh`):

```bash
qemu-system-arm -M vexpress-a9 -m 256M -nographic \
  -kernel u-boot/u-boot \
  -dtb boot-files/vexpress-v2p-ca9.dtb \
  -device loader,file=boot-files/zImage,addr=0x60100000 \
  -drive file=boot-files/rootfs.ext2,if=sd,format=raw
```

This boots cleanly to a `buildroot login:` prompt (`root`, no password).

## Phase 4: TFTP boot path (alternate/learning exercise)

Set up as an additional bring-up skill, closer to real hardware workflows
(rebuild kernel -> drop in TFTP dir -> reboot, no re-flashing).

```bash
sudo mkdir -p /var/lib/tftpboot
sudo cp boot-files/{zImage,vexpress-v2p-ca9.dtb} /var/lib/tftpboot/
sudo systemctl enable --now tftp.socket
```

QEMU launched with usermode networking's built-in TFTP proxy:
```bash
qemu-system-arm -M vexpress-a9 -m 256M -nographic \
  -kernel u-boot/u-boot \
  -drive file=boot-files/rootfs.ext2,if=sd,format=raw \
  -net nic -net user,tftp=/var/lib/tftpboot
```

In U-Boot:
```
setenv ipaddr 10.0.2.15
setenv serverip 10.0.2.2
setenv netmask 255.255.255.0
ping 10.0.2.2
tftpboot 0x60100000 zImage
tftpboot 0x60000000 vexpress-v2p-ca9.dtb
fdt addr 0x60000000        # succeeded immediately - tftpboot respects explicit addresses
setenv bootargs console=ttyAMA0,38400n8 root=/dev/mmcblk0 rootfstype=ext2 rw
bootz 0x60100000 - 0x60000000
```

QEMU usermode network defaults used: guest IP `10.0.2.15`, gateway/host proxy
`10.0.2.2` (these are QEMU's fixed SLIRP addresses, not configurable without
switching network backends).

## Phase 5: Enabling wireless in the kernel config

**Debugging note:** `make linux-menuconfig` search (`/mac80211_hwsim`,
`/CFG80211`, `/WIRELESS`) returned "No matches found" for every wireless-related
symbol, despite the Kconfig files genuinely existing in the kernel source
(confirmed via `find . -iname Kconfig -path "*wireless*"` and
`grep "config WIRELESS" net/Kconfig`).

Root cause: Kconfig hides options whose dependencies aren't satisfied, from
*search* as well as the menu tree. The `vexpress_defconfig` starting point had
`WIRELESS` itself disabled (unsurprising — the real board has no wireless
hardware), which meant `cfg80211`, `mac80211`, and `mac80211_hwsim` were all
structurally hidden until `WIRELESS` was manually enabled.

Fix — manual menu navigation (not search) to enable dependencies in order:

```
Networking support ---> Wireless ---> 
  [*] Wireless                       (parent menu, enables the subtree)
  <M> cfg80211 - wireless configuration API
  <M> Generic IEEE 802.11 Networking Stack (mac80211)

Device Drivers ---> Network device support ---> Wireless LAN ---> 
  [*] Wireless LAN                   (parent menu)
  <M> Simulated radio testing tool for mac80211    <- this is mac80211_hwsim
```

All three enabled as loadable modules (`<M>`), not built-in, so they can be
`modprobe`d explicitly at runtime for a clear, observable demonstration.

## Phase 6: Buildroot userspace wireless packages

```
make menuconfig
  Target packages ---> Networking applications --->
    [*] iw
    [*] wpa_supplicant
    [*] hostapd    (added in a second pass, after discovering it was needed for AP mode)
```

```bash
make    # incremental rebuild, no toolchain rebuild needed
```

## Phase 7: Bringing up the simulated wireless link

After booting with the updated rootfs:

```
modprobe cfg80211
modprobe mac80211
modprobe mac80211_hwsim radios=2
```

Result: two independent radios appear —
```
iw dev
  phy#0 / wlan0  addr 02:00:00:00:00:00
  phy#1 / wlan1  addr 02:00:00:00:01:00
```

Plus a bonus `hwsim0` monitor interface (auto-created by hwsim for observing
simulated over-the-air traffic).

Bring interfaces up:
```
ip link set wlan0 up
ip link set wlan1 up
```

### Access point (wlan0) via hostapd

Config (`configs/wireless/hostapd.conf`):
```
interface=wlan0
driver=nl80211
ssid=TestNetwork
hw_mode=g
channel=1
wpa=2
wpa_passphrase=testpass123
wpa_key_mgmt=WPA-PSK
wpa_pairwise=CCMP
```

```
hostapd -B configs/wireless/hostapd.conf
# wlan0: AP-ENABLED
```

(`rfkill: Cannot open RFKILL control device` is expected/harmless — no
`/dev/rfkill` node in this minimal rootfs, and irrelevant for a simulated
radio with no physical kill switch.)

### Client (wlan1) via wpa_supplicant

Config (`configs/wireless/wpa_supplicant.conf`):
```
network={
    ssid="TestNetwork"
    psk="testpass123"
    key_mgmt=WPA-PSK
}
```

```
wpa_supplicant -B -i wlan1 -c configs/wireless/wpa_supplicant.conf
```

Result:
```
wlan1: authenticate with 02:00:00:00:00:00
wlan1: authenticated
wlan1: associate with 02:00:00:00:00:00
wlan1: RX AssocResp from 02:00:00:00:00:00 (capab=0x411 status=0 aid=1)
wlan1: associated
```

`status=0` = success. Verified independently via:
```
iw dev wlan1 link
  Connected to 02:00:00:00:00:00 (on wlan1)
  SSID: TestNetwork
  freq: 2412.0
  signal: -30 dBm
```

### IP connectivity proof

`udhcpd`/`dnsmasq` were not present in this minimal rootfs (not added to
Buildroot config), so static IPs were used instead of full DHCP, to keep
focus on proving the wireless link itself rather than adding another package
cycle. This still fully validates the IP-over-wireless data path.

```
ip addr add 192.168.50.1/24 dev wlan0
ip addr add 192.168.50.2/24 dev wlan1
ping -c 4 192.168.50.1
```

```
4 packets transmitted, 4 packets received, 0% packet loss
round-trip min/avg/max = 1.856/9.390/20.434 ms
```

## Debugging techniques used

- `file <binary>` — unreliable for raw ARM binaries (misidentified as "DOS
  executable"); don't trust it for headerless embedded binaries.
- `xxd file.dtb | head` — verify a device tree's magic number (`d00dfeed`)
  directly, independent of any bootloader's interpretation of it.
- `md <addr> <count>` (U-Boot) — raw memory dump, used to confirm/deny whether
  expected data actually landed at an expected RAM address.
- `fdt addr <addr>` (U-Boot) — authoritative validity check for a device tree
  at a given address; more reliable than manual hex inspection.
- QEMU monitor (`telnet` to a `-monitor telnet:...` socket, then
  `info registers`) — inspect live CPU state (PC, PSR) to distinguish a genuine
  hang/fault from a silently-working-but-not-printing system.
- `ps aux | grep qemu` — confirm whether a QEMU process is actually alive
  before assuming a boot is stuck vs. never started.

## Known limitations / not pursued

- Regulatory database (`regulatory.db`) not included — `cfg80211` logs a
  harmless "failed to load regulatory.db" warning; irrelevant for
  simulated-only radios.
- DHCP not demonstrated (static IPs used instead) — `udhcpd`/`dnsmasq` package
  not added to Buildroot config.
- No real hardware / USB Wi-Fi dongle passthrough attempted — deliberately
  out of scope; `mac80211_hwsim` was chosen from the outset for a
  hardware-independent, reproducible demonstration.
