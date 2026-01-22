import { createActionGroup, props } from "@ngrx/store";

export const LabelsAction = createActionGroup({
    source: 'Labels',
    events: {
        LoadLabels: props<{ labels: any }>(),
    }
});