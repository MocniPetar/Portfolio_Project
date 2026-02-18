import { ApplicationConfig, inject, provideAppInitializer, provideBrowserGlobalErrorListeners, provideZoneChangeDetection } from '@angular/core';
import { provideRouter } from '@angular/router';
import { provideHttpClient, withFetch } from '@angular/common/http';

import { routes } from './app.routes';
import { provideClientHydration, withEventReplay } from '@angular/platform-browser';
import { provideStore } from '@ngrx/store';
import { labelsReducer } from './states/labels/labels.reducer';
import { settingsReduces } from './states/appsettings/settings.reducer';
import { ConfigService } from './services/config.service';

export const appConfig: ApplicationConfig = {
  providers: [
    provideBrowserGlobalErrorListeners(),
    provideZoneChangeDetection({ eventCoalescing: true }),
    provideRouter(routes), 
    provideClientHydration(withEventReplay()),
    provideHttpClient(withFetch()),
    provideStore({
      labels: labelsReducer,
      settings: settingsReduces
    }),
    provideAppInitializer(() => {
      const configService = inject(ConfigService);

      return Promise.all([
        configService.loadLabels(),
        configService.loadSettings(),
        configService.loadIcons()
      ]);
    })
]
};
