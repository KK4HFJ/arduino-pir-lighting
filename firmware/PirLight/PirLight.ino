// PirLight - PIR motion lighting with an SSR and an AUTO / ON / OFF switch
// Steve Spence (KK4HFJ) - https://kk4hfj.com/
// 2026 cleanup of the Instructable "DIY Arduino PIR Motion Sensor Lighting & Security".
// Same wiring as the original; see README.md for the review notes.
//
//   PIR out -> D2 (PIR powered from +5 V)
//   SSR input + -> D13, input - -> GND (see README: D9 is a better choice)
//   SPDT centre-off switch: centre to +5 V, ends to D11 (AUTO) and D12 (ON), 10k from each to GND
//   Optional LDR divider -> A0 (LDR to +5 V, 10k to GND): no auto-on while it's bright

// ---- user settings ----------------------------------------------------
const uint32_t ON_TIME_MS = 180000UL;   // lights stay on this long after the LAST motion
const uint8_t SSR_PIN = 13;             // D13 flickers during the bootloader; D9 avoids that
#define USE_LDR 0
const int DARK_BELOW = 400;             // A0 reading below this counts as dark (with USE_LDR 1)
// -------------------------------------------------------------------------

const uint8_t PIR_PIN = 2, AUTO_PIN = 11, ON_PIN = 12, LDR_PIN = A0;
uint32_t lastMotion = 0;
bool lightOn = false;

void setLight(bool on) {
  if (on != lightOn) {
    lightOn = on;
    digitalWrite(SSR_PIN, on ? HIGH : LOW);
    Serial.println(on ? F("light on") : F("light off"));
  }
}

void setup() {
  digitalWrite(SSR_PIN, LOW);
  pinMode(SSR_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(AUTO_PIN, INPUT);   // external 10k pull-downs, as in the original
  pinMode(ON_PIN, INPUT);
  Serial.begin(9600);
}

void loop() {
  static bool wasAuto = false;
  bool isAuto = digitalRead(AUTO_PIN) == HIGH;
  if (isAuto && !wasAuto) lastMotion = millis();       // switching to AUTO with the light on: give it a full period
  wasAuto = isAuto;

  if (isAuto) {                                        // AUTO: motion sensor
    if (digitalRead(PIR_PIN) == HIGH) {
#if USE_LDR
      bool dark = analogRead(LDR_PIN) < DARK_BELOW;
#else
      bool dark = true;
#endif
      if (dark || lightOn) {                           // once on, motion keeps it on even if the lamp brightens the LDR
        lastMotion = millis();                         // every detection restarts the timer
        setLight(true);
      }
    }
    if (lightOn && millis() - lastMotion >= ON_TIME_MS) setLight(false);
  } else if (digitalRead(ON_PIN) == HIGH) {            // manual ON
    setLight(true);
  } else {                                             // manual OFF (centre)
    setLight(false);
  }
  delay(20);
}
