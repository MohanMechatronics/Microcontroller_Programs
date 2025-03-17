const int BUZZER_PIN = 12;  // Buzzer signal pin
const int LEDC_CHANNEL = 0;  // LEDC channel for the buzzer
const int LEDC_RESOLUTION = 8;  // LEDC resolution (bit depth)
const int LEDC_FREQUENCY = 2000;  // LEDC PWM frequency

const int NOTE_REST = 0;
const int NOTE_C4 = 262;
const int NOTE_D4 = 294;
const int NOTE_E4 = 330;
const int NOTE_F4 = 349;
const int NOTE_G4 = 392;
const int NOTE_A4 = 440;
const int NOTE_B4 = 494;
const int NOTE_C5 = 523;
const int NOTE_D5 = 587;
const int NOTE_E5 = 659;
const int NOTE_F5 = 698;
const int NOTE_G5 = 784;
const int NOTE_A5 = 880;

const int melody[] = {
  NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_C5, NOTE_B4,
  NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_D5, NOTE_C5,
  NOTE_G4, NOTE_G4, NOTE_G5, NOTE_E5, NOTE_C5, NOTE_B4, NOTE_A4,
  NOTE_F5, NOTE_F5, NOTE_E5, NOTE_C5, NOTE_D5, NOTE_C5
};

const int noteDurations[] = {
  4, 4, 8, 8, 8, 2,
  4, 4, 8, 8, 8, 2,
  4, 4, 8, 8, 8, 8, 2,
  4, 4, 8, 8, 8, 2
};

void ledcSetup()
{
  ledcSetup(LEDC_CHANNEL, LEDC_FREQUENCY, LEDC_RESOLUTION);
  ledcAttachPin(BUZZER_PIN, LEDC_CHANNEL);
}

void playNote(int note, int duration)
{
  ledcWriteTone(LEDC_CHANNEL, note);
  delay(duration);
  ledcWriteTone(LEDC_CHANNEL, 0);
  delay(50);  // Add a small pause between notes
}

void setup()
{
  pinMode(BUZZER_PIN, OUTPUT);
  ledcSetup();
}

void loop()
{
  for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++) {
    int noteDuration = 1000 / noteDurations[i];
    playNote(melody[i], noteDuration);
  }
}