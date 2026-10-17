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

This section covers the T-Watch S3 builds: how the watch differs from the other boards, how to control it by touch, and what every screen and sub-menu does. For the pet itself (growth, meals, hygiene, sleep, DX visits and breeding), see [Getting Started](Getting_Started.md); the rules are the same on every board.

#### What is different on the watch

- **Touch controls.** The watch has a touch screen and a single side button, so it uses taps and swipes instead of the click, double-click and long-press of the other boards (see Controls below).
- **Full-screen colour aquarium.** The aquarium fills the screen with the Cardputer's colour tank scenery (water, light, reef, sand and seaweed). The fish wanders around the tank: it glides to new spots, hovers with a few bubbles, and now and then darts.
- **Other screens are centred**, with their usual 128-pixel-wide layout scaled up to the width of the 240x240 screen.
- **No buzzer**, so the Sound menu is hidden.
- **No GNSS receiver**, so the GNSS page and the GNSS monitor option are left out. To put a position in your adverts, see Position in adverts below.

#### Builds

| Watch | Build environment | Notes |
| --- | --- | --- |
| T-Watch S3 (SX1262) | `TWatch_S3_LoRaFish_companion_ble` | MeshCore BLE companion for the phone app. 80 MHz, aquarium at about 30 frames a second. |
| T-Watch S3 (SX1262) | `TWatch_S3_LoRaFish_standalone` | No Bluetooth, for longer battery life. Drops the CPU to 40 MHz once booted, pauses briefly while the screen is off, and keeps the aquarium at 10 frames a second. |
| T-Watch S3 (SX1280, 2.4 GHz) | `TWatch_S3_1280_LoRaFish_companion_ble` | MeshCore BLE companion. 80 MHz, aquarium at about 30 frames a second. Starts on 2400.5 MHz / 406.25 kHz / SF11 / CR8. |

Build and flash over USB with, for example:

```bash
pio run -e TWatch_S3_LoRaFish_companion_ble -t upload
```

Without a phone app, the no-Bluetooth build takes its time from the watch's clock chip, which keeps time across reboots and reflashing once it has been set (for example from the MeshCore app while running the BLE build). Both SX1262 builds share the same stored data, so you can switch between them without losing your fish, contacts or settings.

#### Controls

| Gesture | Main screens (Aquarium to RF Monitor) | Lists (DX Visit, Options, Display) | Setting screens |
| --- | --- | --- | --- |
| Tap the top area (above the header, or the header itself) | Back: DX Visit goes to Options, the others go to the Aquarium | Back to the previous screen | Back to the parent menu (on the new-fish colour screen: confirm the fish) |
| Tap the right half | Next screen | Next item | Change the value forward |
| Tap the left half | Previous screen | Previous item | Change the value backward |
| Swipe left / right | Next / previous screen | Next / previous screen | - |
| Swipe down / up | - | Next / previous item | Next / previous row |
| Hold a finger still for about a second | The screen's action (for example, clean the tank) | Open the item, or send or recall a DX visit | The screen's action (see each screen below) |
| Short press of the side button | Next screen (and wakes the screen) | Next item | Change the value forward |

- **Changing a setting:** on a setting screen with several rows (for example Schedule & time), swipe down or up to select the row you want, then tap the right half to increase the value or the left half to decrease it.
- **Waking the watch:** taps are ignored while the screen is off, so a sleeve cannot wake it. Press the side button to wake it.
- **Power:** hold the side button for 2 seconds to turn the watch on, and for 6 seconds to turn it off. When the battery runs low (about 3.1 V) the watch switches itself off, unless it is charging.
- **Hints on screen:** the footer of each screen except the aquarium shows "<tap back" and "tap forward>". Most other hints on the screens (CLICK, LONG, 2X) are shared with the single-button boards. On the watch, read CLICK as a tap on the right half and LONG as a hold. 2X usually means a tap on the top area, except on Schedule & time and Screen timeout, where "2X: NEXT" means a swipe down to the next row.
- **Rescue mode:** holding a finger on the screen during the first 8 seconds after start-up opens MeshCore's serial rescue mode, the same as a long press on the other boards.

#### Main screens

Tap the right half or swipe left to move through the main screens in this order:

1. **Aquarium.** Your fish in its tank. The header shows the node name, battery and time; the footer shows the fish's stage, the number of packets heard (PKT) and unread messages (UNR). Pop-ups appear when something happens on the mesh: "* MESSAGE *" for a received message, "* SENT *" for an acknowledged send, "* HELLO! *" for a new contact, and "* EVOLVED! *" when the fish grows. **Hold** to clean the tank when there is waste in it.
   - *Egg:* a new fish starts as an egg, with a grey "HATCHING IN m:ss" countdown above the footer. It hatches after 5 minutes.
   - *Sleeping:* during the sleep hours set in Schedule & time, the fish sleeps and the footer reads "Sleeping".
   - *Visitor:* a fish visiting from another LoRaGotchi DX node swims alongside yours, with its name shown.
   - *Memorial:* after 7 days the fish reaches the end of its life. Use Options, Reset pet to start again, or wait for a bred egg to hatch.
2. **Stats.** Age, happiness, generation (G1, G2 and so on), mesh events (RF), messages (Msg), acknowledgements (ACK), and whether a phone is connected (BLE ON or OFF).
3. **Clock.** Local time and date, using the UTC offset from Schedule & time. The header shows PHONE while the app is connected and MESH otherwise. The bottom line reads "RTC + LOCAL OFFSET" once the clock has been set, or "build-time fallback" if it has not.
4. **Phone pairing.** The Bluetooth PIN for pairing with the MeshCore app (123456 by default). The no-Bluetooth build has nothing to pair, so it shows "No PIN configured".
5. **DX Visit.** Send your fish to visit another LoRaGotchi DX node. Tap the right or left half, or swipe down or up, to choose a contact; **hold** to send the fish. While it is away the screen reads "FISH AWAY"; **hold** again to call it home. Other messages: "DELIVERING...", "CALLING...", "NO REPLY - RETRY" and "NO CONTACTS". A sleeping fish cannot travel. Tap the top area to go to Options.
6. **Options.** The settings menu (see below). Swipe down or up, or tap, to move the highlight; **hold** to open the highlighted item.
7. **RF Monitor.** Signal strength (RSSI) and signal-to-noise ratio (SNR) of the last packet heard, a graph of recent signal strength, the battery level, and the time since the last packet.

#### Options

| Item | What it does |
| --- | --- |
| Display | Opens the Display menu (below). |
| Reset pet | **Hold** to start a new egg. This resets the pet's progress and generation but keeps your display and schedule settings. You then choose the new fish's colours. |
| RF monitor | Opens the RF Monitor screen. |
| Schedule & time | Three rows: **Sleep** and **Wake** set the hours of the fish's nightly sleep (set them the same to keep it awake all day), and **Offset** sets your time zone as an offset from UTC, in 15-minute steps. Swipe down or up to pick a row, then tap right to increase or left to decrease. There is no automatic daylight saving: change the offset when your clocks change. |
| Guess game | Five rounds of higher or lower. Tap to switch between HIGHER and LOWER, then **hold** to guess whether the next number will be higher or lower. Each correct guess adds one happiness point. After five rounds, **hold** to play again. |
| Breed with visitor | Only appears when your fish and a visiting fish are both Adult or Elder and no egg is already waiting. **Hold** to store an egg whose colours mix both parents. When your fish reaches the end of its life, the egg waits beside the gravestone and hatches 5 minutes later as the next generation. |

#### Display menu

Swipe down or up, or tap, to move the highlight; **hold** to open the highlighted item. On each of these screens, tap the top area to go back; holding also goes back, except on Fish color.

| Item | What it does |
| --- | --- |
| Screen timeout | Three rows: **Fish Awake** and **Fish Asleep** set how long the screen stays on (15s, 30s, 1m, 2m, 3m or Always) while the fish is awake or asleep. **Always-On-Dim** (15s, 30s or 60s) sets how soon an always-on screen dims when you are not using it. Swipe down or up to pick a row, then tap right or left to change it. |
| Brightness | 25%, 50%, 75% or 100%. |
| Firmware theme | Midnight, Paper, Lagoon or Sunset. |
| Fish color | The colour picker for the fish's Scales, Body / Border and Eyes: tap right or left to change the colour, and **hold** to move to the next part (or swipe down or up to move between parts). The colours are Coral, Tangerine, Gold, Mint, Lagoon, Orchid and Mono. When you start a new fish this screen appears on its own as "NEW FISH": tap the top area when you are happy with the colours. |
| Game rendering | **Smooth** or **Simulation**: how often the aquarium is redrawn. Simulation redraws less often. |

#### Position in adverts

The watch has no GNSS receiver, but it can still share a position. On a BLE build, set your position in the MeshCore app and turn on location sharing in adverts there. The watch saves the position, keeps it after a reboot, and includes it in its adverts.

#### Credits

The hardware support (power, display, touch, side button and the SX1280 radio) is ported from the T-Watch edition of [Meck](https://github.com/pelgraine/Meck) by pelgraine.

The project tree also retains the wider upstream MeshCore targets and infrastructure from which LoRaGotchi is derived.

## Project history

Development notes from the experimental LoRaFish/LoRaGotchi builds are retained under [`docs/development/`](docs/development/) for reference. The v1.0 release note is under [`docs/release/`](docs/release/).

## Credits

LoRaGotchi DX is by **Robert Rayner / M0YNW** and is based on **MeshCore** by Scott Powell / Ripple Radios. MeshCore provides the underlying mesh transport and substantial portions of the firmware foundation.

See [`NOTICE`](NOTICE) for attribution details.

## License

MIT. See [`LICENSE`](LICENSE).