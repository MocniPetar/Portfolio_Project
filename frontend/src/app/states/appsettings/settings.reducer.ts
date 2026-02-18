import { createReducer, on } from "@ngrx/store";
import { SettingsAction } from "./settings.actions";

export const initialState = {};

export const settingsReduces = createReducer(
    initialState,
    on(SettingsAction.loadSettings, (state, { settings }) => {
        return { ...state, ...settings};
    })
);