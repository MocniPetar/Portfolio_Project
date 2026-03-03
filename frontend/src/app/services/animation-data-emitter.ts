import { Injectable } from '@angular/core';
import { BehaviorSubject, Subject } from 'rxjs';
import { animationData } from '../models/animation';

@Injectable({
  providedIn: 'root',
})
export class AnimationDataEmitter {
  private dataStore = new BehaviorSubject<animationData>({mainText: "", continuteText: "", animationDuration: 0, animationDelay: 0});
  private triggerAnimationChange = new Subject<void>();

  currentData = this.dataStore.asObservable();
  triggerAction$ = this.triggerAnimationChange.asObservable();

  changeAnimationData(newData: animationData) {
    this.dataStore.next(newData);
  }

  callIntroAnimationFunction() {
    this.triggerAnimationChange.next();
  }
}
