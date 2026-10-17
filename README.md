# LoRaGotchi DX 🐟📡

**LoRaGotchi DX v1.0 — M0YNW** is a virtual LoRa fish built on the MeshCore transport. Your fish grows through pet stages, lives in a small aquarium UI, and can travel over the mesh to visit another LoRaGotchi device before being recalled home.

For setup, controls, menus, pet behavior, and DX travel, see [Getting Started](Getting_Started.md).

## v1.0 highlights
- LoRaGotchi virtual-pet aquarium and evolution/gameplay
- DX fish visiting using the LFG1 protocol
    - LFG1 protocol upgraded to include colour information
    - Improved the visiting and eating animations
    - Send, acknowledge, commit, arrival confirmation and recall/return flow
    - Host and visiting fish can coexist and swim independently
    - Visiting fish keeps its own evolution stage
    - A fish away from it's own tank will no longer poop in it's own tank
    - Travel is locked while the local fish is asleep
    - DX session state is cleared on reboot as a recovery mechanism
- LoRaGotchi / M0YNW boot branding
- MeshCore remains the underlying radio/mesh transport
- Buzzer support
- Colour screen support
- Higher/Lower game - 5 rounds. each win = 1 happy at the end of the game (Max +5 happy)

## Testing features:
- Lifecycle improvements, basic fish-colour DNA, breeding & self-fertilization
    - Your fish now has generations > everyone starts with a 1st gen fish. old-age death no longer ends the LoRagotchi game
    - Your game will ask for what colour your fish should start as. breeding can evolve the fish.
- happiness re-balance
    - Fixed double-tap for happiness from an RF message
    - Added basic time drain
    - Added pooped-in-tank drain.
- M2 Partition change - dropped 2nd app partition in favour of a larger primary app partition.
    - alleviates flash from 95.5% to about 42%, in line with other nodes.
    - retained NVS, Filesystem and Coredump partitions.
    - Disables OTA updates (not used or planned)
- M5Stack CardPuter
    - Updated graphics and keyboard controls for the menu

## Supported LoRaGotchi targets in this tree

| MeshCore Node | Build Environment |
| --- | --- |
| ThinkNode M2 | ThinkNode_M2_LoRaFish_companion_ble |
| Heltec V3 | Heltec_v3_LoRaFish_companion_ble |
| Heltec V4 | heltec_v4_LoRaFish_companion_ble |
| M5Stack CardPuter | cardputer_loragotchi |
| Heltec T114 | Heltec_t114_LoRaFish_companion_ble -t create_uf2 |
| Heltec T096 | Heltec_t096_LoRaFish_companion_ble -t create_uf2 |

| Untested nodes | build Environment |
| --- | --- |
| T-Beam (SX1262) | Tbeam_SX1262_LoRaFish_companion_ble
| T-Beam (SX1276) | Tbeam_SX1276_LoRaFish_companion_ble
| LilyGo T-Watch S3 (BLE) | TWatch_S3_LoRaFish_companion_ble
| LilyGo T-Watch S3 (no Bluetooth) | TWatch_S3_LoRaFish_standalone
| LilyGo T-Watch S3 SX1280 2.4 GHz (BLE) | TWatch_S3_1280_LoRaFish_companion_ble

## Building

Install PlatformIO, clone the repository, then build the required environment from the repository root. For example:

```bash
git clone <your-repository-url>
cd LoRaGotchi-DX
pio run -e [BUILD ENVIRONMENT]
```
⚠️ The build environment is Case Specific!

Examples:
```bash
git clone <your-repository-url>
cd LoRaGotchi-DX
pio run -e Heltec_t114_LoRaFish_companion_ble -t create_uf2
```
The resulting file will end up in `LoRagotchi-DX-main\.pio\build\Heltec_t114_LoRaFish_companion_ble\firmware.uf2`
NRF52 devices, After the build, enter the board's UF2 bootloader and copy `firmware.uf2` to its mounted drive. 
```bash
git clone <your-repository-url>
cd LoRaGotchi-DX
pio run -e heltec_v4_LoRaFish_companion_ble
```
The resulting file will end up in `LoRagotchi-DX-main\.pio\build\heltec_v4_LoRaFish_companion_ble\firmware.bin`
Upload with PlatformIO using the normal `-t upload` target for supported serial-programmed boards. The T096 uses -t create_uf2 and the UF2 procedure above.
the meshcore.io/flasher also works, scroll to the bottom and select custom device firmware!

### Buzzer pins

LoRaFish buzzer outputs are assigned per board:

| Device | Buzzer pin |
| --- | --- |
| ThinkNode M2 | GPIO5 |
| Heltec WiFi LoRa 32 V3 | GPIO6 |
| Heltec WiFi LoRa 32 V4 OLED | GPIO6 |
| Heltec T114 | GPIO46 / P1.14 |
| Heltec T096 | GPIO45 / P1.13 |

### LilyGo T-Watch S3

The watch has no buzzer, so the Sound menu is hidden. The screens keep their usual 128-pixel-wide layout, scaled up to the full width of the 240x240 screen. The aquarium uses a taller tank that fills the screen, with the fish swimming in the middle, and the Cardputer's colour tank scenery (water, light, reef, sand and seaweed). The fish wanders around the tank, gliding to new spots, hovering with a few bubbles and now and then darting. The other screens are centred.

The T-Watch S3 has no GNSS receiver, so the GNSS page and the GNSS monitor option are left out. To include a position in adverts, set it in the MeshCore app over Bluetooth and turn on location sharing in adverts there.

The watch uses directional taps and swipes:

| Gesture | Main screens (Aquarium to RF Monitor) | Lists (DX Visit, Options, Display) | Setting screens |
| --- | --- | --- | --- |
| Tap the top area (above the header, or the header itself) | Back: DX Visit goes to Options, the others go to the Aquarium | Back to the previous screen | Back to the parent menu (on the new-fish colour screen: confirm the fish) |
| Tap the right half | Next screen | Next item | Change the value forward |
| Tap the left half | Previous screen | Previous item | Change the value backward |
| Swipe left / right | Next / previous screen | Next / previous screen | - |
| Swipe down / up | - | Next / previous item | Next / previous field |
| Hold a finger still for about a second | Long-press action (for example, clean the tank) | Long-press action (open the item, send or recall a DX visit) | Long-press action |
| Short press of the side button | Next (and wakes the screen) | Next item | Change the value forward |

The on-screen hints (CLICK, LONG, 2X) describe the single-button controls used on other boards. Taps are ignored while the screen is off; press the side button to wake it. Holding the side button for 6 seconds is a hardware power-off.

There are two builds:

- `TWatch_S3_LoRaFish_companion_ble` is a normal MeshCore BLE companion, running at 80 MHz, with the aquarium at about 30 frames a second.
- `TWatch_S3_LoRaFish_standalone` has no Bluetooth, for longer battery life. It drops the CPU to 40 MHz once booted, pauses the main loop briefly on each pass while the screen is off, and keeps the aquarium at 10 frames a second. Without a phone app, the clock comes from the watch's RTC chip, which keeps time across reboots and reflashing once it has been set (for example from the MeshCore app while running the BLE build).
- `TWatch_S3_1280_LoRaFish_companion_ble` is the BLE companion for the 2.4 GHz (SX1280) watch, running at 80 MHz, with the aquarium at about 30 frames a second. It boots on 2400.5 MHz / 406.25 kHz / SF11 / CR8.

The hardware support (power, display, touch and side button) is ported from the T-Watch edition of [Meck](https://github.com/pelgraine/Meck) by pelgraine.

The project tree also retains the wider upstream MeshCore targets and infrastructure from which LoRaGotchi is derived.

## Project history

Development notes from the experimental LoRaFish/LoRaGotchi builds are retained under [`docs/development/`](docs/development/) for reference. The v1.0 release note is under [`docs/release/`](docs/release/).

## Credits

LoRaGotchi DX is by **Robert Rayner / M0YNW** and is based on **MeshCore** by Scott Powell / Ripple Radios. MeshCore provides the underlying mesh transport and substantial portions of the firmware foundation.

See [`NOTICE`](NOTICE) for attribution details.

## License

MIT. See [`LICENSE`](LICENSE).