import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { firstValueFrom } from 'rxjs';
import { Store } from '@ngrx/store';
import { LabelsAction } from '../states/labels/labels.actions';
import { SettingsAction } from '../states/appsettings/settings.actions';

@Injectable({
  providedIn: 'root',
})
export class ConfigService {

  constructor(
    private http: HttpClient,
    private store: Store
  ) {}

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