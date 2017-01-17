void set_LED(int status) {
  LED_status = status;
  if (pinLED >= 0) {
    digitalWrite(pinLED, LED_status);
  }
}

void toggle_LED() {
  LED_status = 1 - LED_status;
  if (pinLED >= 0) {
    digitalWrite(pinLED, LED_status);
  }
}
