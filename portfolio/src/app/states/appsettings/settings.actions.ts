import { createActionGroup, props } from "@ngrx/store";

export const SettingsAction = createActionGroup({
    source: 'Settings',
    events: {
        LoadSettings: props<{ settings: any }>(),
    }
});