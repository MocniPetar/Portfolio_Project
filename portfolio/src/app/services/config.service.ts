import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { firstValueFrom } from 'rxjs';
import { Store } from '@ngrx/store';
import { LabelsAction } from '../states/labels/labels.actions';
import { SettingsAction } from '../states/appsettings/settings.actions';
import { MatIconRegistry } from '@angular/material/icon';
import { DomSanitizer } from '@angular/platform-browser';

@Injectable({
  providedIn: 'root',
})
export class ConfigService {

  constructor(
    private http: HttpClient,
    private store: Store,
    private iconRegistry: MatIconRegistry,
    private sanitizer: DomSanitizer
  ) {}

  loadIcons(): Promise<any> {
    return firstValueFrom(this.http.get<any>('/config/icons.json')).then(
      (icons: any) => {
        Object.entries(icons).forEach(([name, path]) => { // --> Object.entries turns json objects into array of key/values of the enumerable own properties of an object
          this.iconRegistry.addSvgIcon(
            name as string,
            this.sanitizer.bypassSecurityTrustResourceUrl(path as string)
          );
        })
      }
    );
  }

  loadLabels(): Promise<any> {
    return firstValueFrom(this.http.get<any>('/config/labels.json')).then(
      (labels: any) => {
        this.store.dispatch(LabelsAction.loadLabels({ labels }));
      }
    );
  }

  loadSettings(): Promise<any> {
    return firstValueFrom(this.http.get<any>('/config/appsettings.json')).then(
      (settings: any) => {
        this.store.dispatch(SettingsAction.loadSettings({ settings }));
      }
    );
  }
}