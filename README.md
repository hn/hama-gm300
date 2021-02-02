# hama GM-300 garden monitor

## Preamble

The [hama GM-300 garden monitor](https://de.hama.com/00106999/hama-gm-300-garden-monitor) displays
the temperature of the air and the humidity and temperature of the soil. Up to five sensors can be
connected to the base station.

## Protocol

The sensors communicate via 433MHz [OOK](https://en.wikipedia.org/wiki/On%E2%80%93off_keying)
protocol with the base station. The transmission starts with a 6000 microseconds
[synchronization pulse](https://github.com/sui77/rc-switch/wiki/KnowHow_LineCoding),
followed by 64 bits encoded like this: "1" bit = 500 microseconds high + 2000 microseconds low,
"0" bit = 500 microseconds high + 1000 microseconds low.
By using [rc-switch](https://github.com/sui77/rc-switch/wiki/Add_New_Remote_Part_1#adding-a-new-remote-1)'s
definition 'formula' this translates to `{ 500, { 1,  12 }, {  1,  2 }, {  1,  4 }, false }`.

The received telegram (64 bits) can be decoded as follows:

```
HHHHHHHHHHHH-CCC-MB-HH-U-SSSSSSSSSSSS-AAAAAAAAAAAA-UUUUUUUUUUUU-RRRRRRRR
100001001111-100-10-01-0-100111001000-001111110000-000000000000-11110110

H = host id, changes after battery replacement, the upper four bits seem to be "1" fixed
C = channel, as set by the rotary dial on the back of the sensor
M = manual send, if you press the button on the back of the sensor
B = battery low indicator
H = humidity (0, 1, 2)
S = soil temperature, shifted by 10, e.g. 234 = 23.4 deg C
A = air temperature, shifted by 10
U = unknown / unused
R = CRC
```

Thanks to [reveng](https://reveng.sourceforge.io/), the
[CRC](https://en.wikipedia.org/wiki/Cyclic_redundancy_check) is calculated
like this: `width=8  poly=0x31  init=0xff  refin=true  refout=true  xorout=0x00  check=0x0b  residue=0x00`.

## Arduino

[hama-gm300.ino](hama-gm300.ino) can be used to receive and decode telegrams:

```
Received 7BF402F077000000 / 64bit Protocol: 1
hostid:   4062
channel:  2
manual:   0
lowbat:   0
humidity: 0
soiltemp: 244
airtemp:  238
```

## Credits

This projekt ist based on the [rc-switch](https://github.com/sui77/rc-switch/)
library, extended with [64bit support](https://github.com/sui77/rc-switch/pull/221/files).

