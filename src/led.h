void set_LED(int status) {
  LED_status = status;
  digitalWrite(pinLED, LED_status);
}

void toggle_LED() {
  LED_status = 1 - LED_status;
  digitalWrite(pinLED, LED_status);
}
