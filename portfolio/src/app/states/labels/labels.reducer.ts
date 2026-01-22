import { createReducer, on } from "@ngrx/store";
import { LabelsAction } from "./labels.actions";

export const initialState = {};

export const labelsReducer = createReducer(
    initialState,
    on(LabelsAction.loadLabels, (state, { labels }) => {
        return { ...state, ...labels};
    })
);