#ifndef LEDS_H_
#define LEDS_H_

#define LED_YELLOW (1 << 27)
#define LED_RED    (1 << 0)
#define LED_GREEN  (1 << 1)
#define LED_ALL    (LED_YELLOW | LED_RED | LED_GREEN)

void leds_init(void);
void leds_on(unsigned int leds);
void leds_off(unsigned int leds);

#endif