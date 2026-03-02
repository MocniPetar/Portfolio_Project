import { Injectable } from '@angular/core';
import { Subject } from 'rxjs';

@Injectable({
  providedIn: 'root',
})
export class ComponentServ {

  private triggerComponentRefresh = new Subject<void>();
  triggerRefresh$ = this.triggerComponentRefresh.asObservable();

  callRefreshFunction() {
    this.triggerComponentRefresh.next();
  }
}
