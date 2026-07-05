#ifndef LIGHTEFFECTS_H
#define LIGHTEFFECTS_H

// Called every loop() iteration while effectsModeActive is true. Paces
// itself internally (~40ms/frame, non-blocking) - safe to call unconditionally
// on every loop() pass. Dispatches to one of LIGHT_EFFECT_OPTIONS_COUNT
// continuous WLED-style effect functions based on selectedLightEffect.
void renderLightEffects();

#endif // LIGHTEFFECTS_H
