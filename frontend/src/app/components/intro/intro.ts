import { AfterViewInit, Component, Output, EventEmitter, HostListener, OnInit, Input, OnDestroy } from '@angular/core';
import { CommonModule } from '@angular/common';
import { AnimationDataEmitter } from '../../services/animation-data-emitter';
import { Subscription } from 'rxjs';
import { Router } from '@angular/router';
import { ComponentServ } from '../../services/component';

@Component({
  selector: 'intro',
  imports: [CommonModule],
  templateUrl: './intro.html',
  styleUrl: './intro.less',
})
export class Intro implements OnInit, AfterViewInit, OnDestroy {

  @Output() changed = new EventEmitter<boolean>()

  private toolbarSubscription: Subscription = new Subscription();
  private animationChangeSubscription: Subscription = new Subscription();
  private refreshFuncSubscription: Subscription = new Subscription();

  firstLoad = false;
  animationEnd = false;
  animationTextArray: string[] = [];
  recievedData: any;

  constructor(
    private dataService: AnimationDataEmitter,
    private router: Router,
    private componentService: ComponentServ
  ){}

  @HostListener('document:click', ['$event'])
  handleClick(event: MouseEvent) {
    if (this.animationEnd)
      this.changed.emit(true);   
  }

  ngOnInit(): void {
    this.animationChangeSubscription = this.dataService.currentData.subscribe(data => {
      this.recievedData = data;
    });

    this.toolbarSubscription = this.dataService.triggerAction$.subscribe(data => {
      this.changeAnimationText();
    });

    this.refreshFuncSubscription = this.componentService.triggerRefresh$.subscribe(() => {
      this.refreshViewport();
    });

    if (this.recievedData) { this.animationTextArray = this.recievedData.split(''); }
  }

  ngAfterViewInit(): void {
    requestAnimationFrame(() => {
        this.firstLoad = true;
    });
  }

  changeAnimationText() {
    if (typeof this.recievedData === 'string') {
      this.animationTextArray = this.recievedData.split('');
    }
  }

  onAnimationEnd() {
    this.animationEnd = true;
  }

  refreshViewport() {
    const currentUrl = this.router.url;
    this.router.navigateByUrl('/refresh', { skipLocationChange: true }).then(() => {
      this.router.navigate([currentUrl]);
    });
  }

  ngOnDestroy(): void {
    this.toolbarSubscription.unsubscribe();
    this.animationChangeSubscription.unsubscribe();
    this.refreshFuncSubscription.unsubscribe();
  }
}
