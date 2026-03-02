import { Injectable } from '@angular/core';
import { BehaviorSubject, Subject } from 'rxjs';

@Injectable({
  providedIn: 'root',
})
export class AnimationDataEmitter {
  private dataStore = new BehaviorSubject<string>("");
  private triggerAnimationChange = new Subject<void>();

  currentData = this.dataStore.asObservable();
  triggerAction$ = this.triggerAnimationChange.asObservable();

  changeAnimationData(newData: string) {
    this.dataStore.next(newData);
  }

  callIntroAnimationFunction() {
    this.triggerAnimationChange.next();
  }
}
