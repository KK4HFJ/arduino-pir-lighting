# DIY Arduino PIR Motion Sensor Lighting & Security

PIR motion sensor switches a lamp through a solid-state relay, with an AUTO / ON / OFF override switch and an optional light sensor so it only comes on when it's dark.

Instructable: [DIY Arduino PIR Motion Sensor Lighting & Security](https://www.instructables.com/DIY-Arduino-Motion-Sensor-Lighting-Control/)
Author: Steve Spence, KK4HFJ — <https://kk4hfj.com/>

## Contents

| Path | What it is |
|---|---|
| `firmware/PirLight/PirLight.ino` | Cleaned-up sketch, same wiring. Compiles in CI. |
| `legacy/pir_ssr_2013.ino` | Step 4 sketch as published, unchanged. |

## Hardware (unchanged)

| Part | Connection |
|---|---|
| PIR module (HC-SR501 type) | +5 V, GND, OUT → D2 |
| SSR (DC input, AC output, zero-cross) | input + → D13, input − → GND; output in series with the **hot** lead to the lamp |
| SPDT centre-off switch | centre → +5 V, ends → D11 (AUTO) and D12 (ON), 10 kΩ from each end to GND |
| LDR, optional | LDR from +5 V to A0, 10 kΩ A0 → GND; set `USE_LDR 1` |

## Review findings (published code)

| # | Issue | Effect | Fix |
|---|---|---|---|
| 1 | `delay(180000)` after motion | Arduino is blind for 3 minutes: the switch is ignored (can't turn the light off) and motion during that time doesn't extend it, so the light goes off on someone still in the room | Non-blocking timer from the **last** motion; switch read every 20 ms |
| 2 | SSR on D13 | The bootloader blinks D13 at every reset/power-up, so the lamp flickers | Kept on D13 for compatibility; set `SSR_PIN 9` and move the wire to avoid it |
| 3 | LDR option described but not coded | — | Added (`USE_LDR`), with a latch so the lamp lighting the LDR doesn't switch itself off |
| 4 | Switching from ON to AUTO | (new design) would turn off at once | Timer restarts when AUTO is selected |

## Safety

The SSR output is at mains voltage. Mount it in an enclosure, fuse the circuit, and keep the low-voltage wiring separate. Most cheap SSRs leak a few mA when off, which can make LED bulbs glow faintly; a bleeder resistor or a small mechanical relay fixes that.

## License

MIT — see [LICENSE](LICENSE).
